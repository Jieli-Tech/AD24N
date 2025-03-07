
#include "cpu.h"
#include "config.h"
#include "typedef.h"
/* #include "decoder_api.h" */
/* #include "dev_manage.h" */
#include "audio.h"
#include "dac.h"
#include "sound_effect_api.h"
#include "circular_buf.h"
#include "mio_api.h"
/* #include "energe_api.h" */


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

/* 数据流中断sound，通过各个模式注册进来使用 */
sound_out_obj  *stream_sound;
static void (*stream_kick)(void) = NULL;

void sound_output_hook(void *priv, void *data, int len)
{
    sound_out_obj *psound = priv;
    d_mio_start(psound->mio);

    /* energe_run_api(data, len); */
}

//出数接口,data是出数的起始地址，len是长度（byte），priv为传入的
int sound_output(void *priv, void *data, int len)
{
    sound_out_obj *psound = priv;
    void *obuf;
    u32 dlen = 0;
    if (B_DEC_EFFECT & psound->enable) {
        /* debug_puts("A"); */
        EFFECT_OBJ *e_obj;
        e_obj = (void *)psound->effect;
        if (NULL != e_obj) {
            /* debug_puts("B"); */
            dlen = e_obj->run(e_obj->p_si, data, len);
            goto __mp_output_end;
        }
        /* debug_puts("C"); */
    }
    /* debug_puts("D"); */
    obuf = psound->p_obuf;
    dlen = cbuf_write(obuf, data, len);

    sound_output_hook(psound, data, len);
__mp_output_end:
    return dlen; //返回输出了多少个bye，没输完的下次继续输出
}

int sound_input(void *priv, void *data, int len)
{
    sound_out_obj *psound = priv;
    void *obuf;
    u32 dlen = 0;
    if (B_DEC_RUN_EN & psound->enable) {
        /* debug_puts("D"); */
        obuf = psound->p_obuf;
        dlen = cbuf_read(obuf, data, len);
    }
    return dlen; //返回输出了多少个bye，没输完的下次继续输出
}

/*----------------------------------------------------------------------------*/
/**@brief   数据流kick函数
   @param   _sound:注册进来的sound
   @author
   @note    void kick_sound(void *_sound)
*/
/*----------------------------------------------------------------------------*/
AT(.audio_a.text.cache.L2)
void kick_sound(void *_sound)
{
    sound_out_obj *sound = (sound_out_obj *)_sound;
    if (sound && sound->p_obuf) {
        u32 size = cbuf_get_data_size(sound->p_obuf);
        if (size >= 64) {
            sound->enable |= B_DEC_KICK;
            bit_set_swi(3);
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   数据流音效处理函数
   @author
   @note    void speaker_soft3_isr()
*/
/*----------------------------------------------------------------------------*/
SET(interrupt(""))
void speaker_soft3_isr()
{
    bit_clr_swi(3);
    sound_out_obj *sound = stream_sound;
    if (NULL == sound) {
        log_error("soft3 sound null\n");
        return;
    }

    if (0 == (sound->enable & B_DEC_RUN_EN)) {
        log_error("soft3 sound not run\n");
        return;
    }

    if (sound->enable & B_DEC_KICK) {
        sound->enable &= ~B_DEC_KICK;
        u32 rlen = 0;
        u32 wlen = 0;
        s16 *data = cbuf_read_alloc(sound->p_obuf, &rlen);
        if (rlen) {
            wlen = sound_output(sound, data, rlen);
        }
        cbuf_read_updata(sound->p_obuf, wlen);
        if (NULL != stream_kick) {
            stream_kick();
        }
    }

}

/*----------------------------------------------------------------------------*/
/**@brief   数据流stream_sound注册函数
   @param   psound:注册进来的sound
   @return  当前数据流stream_sound为空，注册成功返回真
            当前数据流stream_sound不为空 或 注册进来的sound为空，返回失败
   @author  liuhaokun
   @note    bool regist_stream_sound(void *psound)
*/
/*----------------------------------------------------------------------------*/
bool regist_stream_sound(void *psound, void *kick)
{
    if ((NULL != stream_sound) && (NULL == psound)) {
        log_error("stream_sound exist or psound is null\n");
        return false;
    } else {
        stream_sound = psound;
        stream_kick = kick;
    }
    return true;
}

/*----------------------------------------------------------------------------*/
/**@brief   数据流stream_sound注销函数
   @param   _sound:注册进来的sound
   @author  liuhaokun
   @note    bool unregist_stream_sound(void)
*/
/*----------------------------------------------------------------------------*/
void unregist_stream_sound(void)
{
    stream_sound = NULL;
    stream_kick = NULL;
}


/*----------------------------------------------------------------------------*/
/**@brief   数据流stream_sound初始化函数
   @param   _sound:注册进来的sound
   @author  liuhaokun
   @note    bool stream_sound_init(void *psound)
*/
/*----------------------------------------------------------------------------*/
void stream_sound_init(void *psound, void *kick)
{
    HWI_Uninstall(IRQ_SOFT3_IDX);
    HWI_Install(IRQ_SOFT3_IDX, (u32)speaker_soft3_isr, IRQ_DECODER_IP);

    regist_stream_sound(psound, kick);
}

/*----------------------------------------------------------------------------*/
/**@brief   数据流stream_sound重置函数
   @author  liuhaokun
   @note    bool stream_sound_uninit(void)
*/
/*----------------------------------------------------------------------------*/
void stream_sound_uninit(void)
{
    HWI_Uninstall(IRQ_SOFT3_IDX);
    unregist_stream_sound();
}
