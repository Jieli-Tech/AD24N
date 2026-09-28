#include "config.h"
#include "audio.h"
#include "app_config.h"
#include "circular_buf.h"
#include "errno-base.h"
#include "aec_audio.h"
#include "src.h"
#include "src_api.h"
#include "lib_aec.h"
#include "audio_adc.h"
#include "audio_dac_cpu.h"
#include "src_to_buffer.h"
#include "seq_buf.h"
#include "circular_dac_recode.h"

#if (defined HAS_AEC_MODE)

#define NLP_EN 1

#endif

#if ((defined NLP_EN) && (NLP_EN))

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[nlp]"
#include "log.h"

/*
 * AEC STATE BITMAP
 */
#define AEC_STATE_INIT			BIT(0)	/*AEC aec task ready	*/
#define AEC_STATE_ADC			BIT(1)	/*AEC adc ready			*/
#define AEC_STATE_SUSPEND		BIT(5)	/*AEC aec task suspend	*/
#define AEC_STATE_OK			(AEC_STATE_INIT | AEC_STATE_ADC)

#define NS_MODE                 0
#define NS_IS_WIDEBAND          1//0:8K 1:16K
#define NS_D                    (64 << NS_IS_WIDEBAND)

#define ECHOSP_RUN_BUFSIZE      (0)
#define COMM_RUNBUF_SIZE        (ECHOSP_RUN_BUFSIZE)

#define ECHOSP_TMP_BUFSIZE      (0)
#define COMM_TMPBUF_SIZE        ECHOSP_TMP_BUFSIZE

/* AEC_SRE */

typedef struct {
    SRC_BUFF_NOIO *p_src;
    cbuffer_t *p_cbuf;
    SEQ_BUF seq;
} AEC_SRC;

typedef struct {
    volatile unsigned char toggle;
    volatile unsigned char state;
    short output[READSIZE];
    short aec_ref[AEC_REF_POINT];
    AEC_SRC ref_output;

    short aec_near[AEC_NEAR_POINT];
    AEC_SRC near_output;
} AEC_VAR;

static AEC_VAR aec_var ;//AT(HSuo_aec_ram);
static AEC_VAR *aec ;//AT(HSuo_aec_ram);

void log_ref_output_cbuf(char *ptr)
{
    // log_info("%s,0x%x",ptr, (u32)aec->ref_output.p_cbuf);
}

u32 aec_run_src(AEC_SRC *p_aec_src)
{
    //log_char('a');
    if (NULL == p_aec_src->p_src) {
        //cbuf to seq_buf
        s16 *rptr;
        u32 olen, tlen;
        while (1) {
            rptr = cbuf_read_alloc(p_aec_src->p_cbuf, &olen);
            /* log_char('B'); */
            if (0 == olen) {
                if (0 == p_aec_src->seq.data_len) {
                    log_char('E');
                } else {
                    /* log_char('e'); */
                }
                break;
            }
            tlen = seq_buf_write(&p_aec_src->seq, (const u8 *)rptr, olen);
            /* log_char('c'); */
            cbuf_read_updata(p_aec_src->p_cbuf, tlen);
            if (tlen != olen) {
                break;
            }
        }
        return  p_aec_src->seq.data_len;
    } else {
        //log_char('c');
        //cbuf run src to seq_buf;
        return src_run_noio_cbuf(p_aec_src->p_src,
                                 &p_aec_src->seq,
                                 p_aec_src->p_cbuf
                                );
        //log_char('d');
    }
}

/*
// 读取参考数据和近端录音数据
cbuf_read(&aec->near_cbuf, aec->aec_near, AEC_NEAR_POINT * sizeof(short));
cbuf_read(&aec->ref_cbuf, aec->aec_ref, AEC_REF_POINT * sizeof(short));
*/
/* u32 aec_debug_fail; */
/* u32 aec_debug_succ; */

void aec_kick_print()
{
    //log_info("%d,%d",aec_debug_fail,aec_debug_succ);
}



#define ONE_TIME_REF_POINT      (AEC_REF_POINT  * sizeof(short))
#define ONE_TIME_NEAR_POINT     (AEC_NEAR_POINT * sizeof(short))

AT(.audio_d.text.cache.L2)
bool HSuo_kick_aec_run(void *p_sound_out)
{
    // 检查数据是否足够运行一次AEC算法
    // log_info("Hsuo kick run %d,%d",cbuf_get_data_size(aec->near_output.p_cbuf),(AEC_NEAR_POINT * sizeof(short)) );
    if (cbuf_get_data_size(aec->near_output.p_cbuf) < ONE_TIME_NEAR_POINT) {
        // log_char('k');
        return false;
    }
    if (record_dac_len(0) < ONE_TIME_REF_POINT) {
        /* if (record_dac_len(16000) < (READSIZE * sizeof(short))) { */
        /* log_char('K'); */
        /* aec_debug_fail++; */
        return false;
    } else {
        /* aec_debug_succ++; */
    }
    //if (!get_shared_mem_busy()) {
    // log_char('j');
    if (0 == (u32)p_sound_out) {
        log_char('J');
    } else {
        /* log_char('.'); */
    }
    /* aec->state = AEC_STATE_OK; */
    bit_set_swi(2);
    //}
    return true;
}

#include "lib_kws.h"

/* int aec_run(void *hld, short *inbuf, int len) */
int HSuo_aec_run()
{
    /* int nOut = 0; */

    if (aec == NULL) {
        log_error("aec == null");
        return 0;
    }

    /* if (aec->state != AEC_STATE_OK) { */
    /* log_error("aec_state err:%x\n", aec->state); */
    /* return 0; */
    /* } */
    log_ref_output_cbuf("HSuo_aec_run");
    //log_char('R');
    record_dac_read(aec->ref_output.p_cbuf);
    /* log_char('0'); */
    u32 ref_len  = aec_run_src(&aec->ref_output);
    /* log_char('1'); */
    u32 near_len = aec_run_src(&aec->near_output);
    //log_char('2');

    if (ref_len < ONE_TIME_REF_POINT) {
        return 0;
    }

    if (near_len < ONE_TIME_NEAR_POINT) {
        u32 target_len = AEC_NEAR_POINT * sizeof(short);
        /* log_info("dac %d, %d",near_len,target_len); */
        if (target_len != seq_buf_expand(&aec->near_output.seq, target_len)) {
            log_char('X');
        } else {
            log_char('x');
        }
        /* return 0; */
    }
    // log_info(,,);
    short *pFar  = (void *)aec->ref_output.seq.buff;
    short *pNear = (void *)aec->near_output.seq.buff;

    //memset(pFar, 0, 256 * sizeof(short));
    ZBIT_STATUS sta = AI_ECNR_Processing(pNear, pFar, aec->output);
    if (sta != errZBIT_NoError) {
        log_error("AI_ECNR_Processing error: %d\n", sta);
        memcpy(aec->output, pNear, READSIZE * sizeof(short)); // 出错时旁路
    }

    // log_char('3');
    kws_load_input(aec->output, 256);
    /* kws_load_input(aec->output, 128); */
    /* kws_load_input(((short *)aec->output) + 128, 128); */

    int nOut = READSIZE;
    seq_data_used(&aec->ref_output.seq,  nOut * sizeof(short));
    seq_data_used(&aec->near_output.seq, nOut * sizeof(short));
    /* seq_data_used(&aec->ref_output.seq,  ONE_TIME_REF_POINT); */
    /* seq_data_used(&aec->near_output.seq, ONE_TIME_NEAR_POINT); */
    // log_char('4');

    return READSIZE * sizeof(short);
}

SET(interrupt(""))
void soft2_isr(void)
{
    bit_clr_swi(2);
    HSuo_aec_run();

}

/* extern int8_t med_buffer[512 * 4]; */
extern int32_t med_buffer[512];

extern const int IRQ_NLP_IP;
u32 nlp_api(u32 sr)
{
    const u32 nlp_supprt_sr[2] = {8000, 16000};
    log_info("nlp_api !!!!!!!!");
    if (sr != nlp_supprt_sr[NS_IS_WIDEBAND]) {
        log_error("nlp not support curr sr %d\n", sr);
        return 0;
    }

    aec = &aec_var;
    memset(aec, 0, sizeof(AEC_VAR));
    aec->near_output.p_cbuf = regist_aec_mic_2_audio_dac();
    aec->ref_output.p_cbuf  = regist_aec_ref_2_audio_dac();

    //输出的SEQ_BUF初始化
    seq_buf_init(&aec->near_output.seq, (void *)(&aec->aec_near[0]), sizeof(aec->aec_near));
    seq_buf_init(&aec->ref_output.seq,  (void *)(&aec->aec_ref[0]),  sizeof(aec->aec_ref));

    if (SR_DEFAULT != nlp_supprt_sr[NS_IS_WIDEBAND]) {
        //aec优先级高于解码，解码会用到SRC，本系统为逻辑，SRC无互斥；
        //aec运算会和解码同时工作；
        //如果src优先级与解码同级。那么aec能使用SRC。
        log_error("aec dac can't use src");
        return E_AEC_DAC_NEED_SRC;
        /*
        aec->ref_output.p_src = src_noio_api(SR_DEFAULT, nlp_supprt_sr[NS_IS_WIDEBAND]);
        log_info("aec dac need src!!! 0x%x", (u32)aec->ref_output.p_src);
        while (1);*/
    } else {
        log_info("aec dac no need src!!!");
        aec->ref_output.p_src = NULL;
    }

    u32 near_sr = read_audio_adc_sr();
    if (near_sr != nlp_supprt_sr[NS_IS_WIDEBAND]) {
        //aec优先级高于解码，解码会用到SRC，本系统为逻辑，SRC无互斥；
        //aec运算会和解码同时工作；
        //如果src优先级与解码同级。那么aec能使用SRC。
        log_error("aec mic can't use src");
        return E_AEC_ADC_NEED_SRC;
        /*
        log_info("aec adc need src!!!");
        aec->near_output.p_src = src_noio_api(near_sr, nlp_supprt_sr[NS_IS_WIDEBAND]);
        while (1);*/
    } else {
        log_info("aec adc no need src!!!");
        aec->near_output.p_src = NULL;
    }

//     初始化 AI_ECNR 算法
    zbit_init_pram_t pram = {
        .sample_rate = (int32_t)sr,
        .shared_mem = (int32_t *)med_buffer,
    };

    ZBIT_STATUS sta = AI_ECNR_init(&pram);
    if (sta != errZBIT_NoError) {
        log_error("AI_ECNR_init failed: %d\n", sta);
        return 0;
    }
    log_info("AI_ECNR version: %s\n", AI_ECNR_Version());

    /* aec_adda_buf_init(); */

    HWI_Install(IRQ_SOFT2_IDX, (u32)soft2_isr, IRQ_NLP_IP);

    aec->state = AEC_STATE_INIT | AEC_STATE_SUSPEND;
    log_info("aec init ok, state=%x\n", aec->state);

    enable_HSsuo_ref_sound();
    return 0;
}



#endif /* NLP_EN */
