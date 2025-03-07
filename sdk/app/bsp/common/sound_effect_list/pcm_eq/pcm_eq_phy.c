
#include "typedef.h"
/* #include "decoder_api.h" */
#include "decoder_mge.h"
#include "config.h"
#include "sound_effect_api.h"
#include "my_malloc.h"
#include "pcm_eq.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

typedef struct _PCM_EQ_IO_CONTEXT_ {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} PCM_EQ_IO_CONTEXT;

typedef struct _PCM_EQ_HDL {
    EFFECT_OBJ obj;//必须在第一个
    sound_in_obj si;
    PCM_EQ_IO_CONTEXT io;
    /* ECHO_API_STRUCT echo; */
} PCM_EQ_HDL;

PCM_EQ_HDL pcm_eq_hdl AT(.pcm_eq_data);

typedef struct _PE_PHY {
} PE_PHY_HDL;

typedef struct _PE_DATA {
    u8  *work_buf;
    PCM_EQ_IO_CONTEXT *io;
    u16 remain_len;
    u16 offset;
    u8 obuf[32 * 2];
} PE_DATA_HDL;

/* static PE_PHY_HDL s_pe_phy AT(.pcm_eq_data); */


void pcm_eq_open(void *dbuf, void *io, PCM_EQ_PARM *p_parm)
{
    u8 *p_buf = dbuf;
    PE_DATA_HDL *p_pe_data = dbuf;

    memset(p_pe_data, 0, sizeof(PE_DATA_HDL));

    void *work_buf = p_buf + sizeof(PE_DATA_HDL);
    p_pe_data->work_buf = work_buf;
    p_pe_data->io = io;
    EQInit(work_buf, \
           p_parm->LCoeff_OnChip, \
           p_parm->RCoeff_OnChip, \
           p_parm->LGain, \
           p_parm->RGain, \
           p_parm->SHI, \
           p_parm->SHO, \
           p_parm->nSection, \
           p_parm->channel);

    /* nSection = 10; */
    /* channel = 2; */

    /* EQUpdat(work_buf, \ */
    /* p_parm->LCoeff_OnChip, \ */
    /* p_parm->RCoeff_OnChip, \ */
    /* p_parm->LGain, \ */
    /* p_parm->RGain, \ */
    /* p_parm->SHI, \ */
    /* p_parm->SHO, \ */
    /* p_parm->nSection, \ */
    /* p_parm->channel); */

}

void pcm_eq_updata(void *p_dbuf, PCM_EQ_PARM *p_parm)
{
    PE_DATA_HDL *p_pe_data = (void *)p_dbuf;
    void *work_buf = p_pe_data->work_buf;
    EQUpdate(work_buf, \
             p_parm->LCoeff_OnChip, \
             p_parm->RCoeff_OnChip, \
             p_parm->LGain, \
             p_parm->RGain, \
             p_parm->SHI, \
             p_parm->SHO, \
             p_parm->nSection, \
             p_parm->channel);
}

int pcm_eq_buf_len(int nSection, int channel)
{
    u32 buf_size = getEQBuf(nSection, channel);
    return buf_size + sizeof(PE_DATA_HDL);
}

#define PE_OBUF_LEN   sizeof(p_pe_data->obuf)
static int pcm_eq_run(void *hld, short *inbuf, int len)
{
    sound_in_obj *p_si = hld;
    int res_len = 0;
    PE_DATA_HDL *p_pe_data = (void *)p_si->p_dbuf;

    if (NULL != p_pe_data) {
        u32 tlen = 0;
        int relen = p_pe_data->remain_len;
        if (0 != p_pe_data->remain_len) {
            int offset = p_pe_data->offset;
            tlen = p_pe_data->io->output(p_pe_data->io->priv, &p_pe_data->obuf[offset], relen);
            relen -= tlen;
            if (0 == relen) {
                p_pe_data->remain_len = 0;
                p_pe_data->offset =  0;
                memset(&p_pe_data->obuf[0], 0, PE_OBUF_LEN);
            } else {
                offset += tlen;
                return res_len;
            }
        }

        u8 *t_inbuf = (void *)inbuf;
        u8 *work_buf = p_pe_data->work_buf;
        while (0 != len) {
            tlen = PE_OBUF_LEN > len ? len : PE_OBUF_LEN;
            res_len += tlen;
            len -= tlen;
            /* log_char('R'); */
            EQRun(work_buf, (void *)t_inbuf, (void *)&p_pe_data->obuf[0], tlen / 2);

            t_inbuf += tlen;

            u32 ttlen = p_pe_data->io->output(p_pe_data->io->priv, &p_pe_data->obuf[0], tlen);
            if (ttlen < tlen) {
                p_pe_data->remain_len = tlen - ttlen;
                p_pe_data->offset =  ttlen;
                break;
            }

        }
    }
    return res_len;
}



void *pcm_eq_phy(void *obuf, void *dbuf, PCM_EQ_PARM *parm, void **ppsound)
{

    EFFECT_OBJ *pcm_eq_obj = &pcm_eq_hdl.obj;
    memset((void *) &pcm_eq_hdl, 0, sizeof(pcm_eq_hdl));
    pcm_eq_hdl.io.priv =   &pcm_eq_obj->sound;
    pcm_eq_hdl.io.output = sound_output;

    pcm_eq_hdl.si.ops = 0;
    pcm_eq_hdl.si.p_dbuf = dbuf;

    pcm_eq_obj->p_si = &pcm_eq_hdl.si;
    pcm_eq_obj->run = pcm_eq_run;
    pcm_eq_obj->sound.p_obuf = obuf;
    *ppsound = &pcm_eq_obj->sound;


    //open
    pcm_eq_open(dbuf, &pcm_eq_hdl.io, parm);
    return &pcm_eq_hdl.obj;
}


