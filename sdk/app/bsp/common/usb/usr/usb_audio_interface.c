
#include "usb/device/msd.h"
#include "usb/device/usb_stack.h"
#include "usb/device/uac_audio.h"
#include "usb/usr/usb_audio_interface.h"
#include "uac_sync.h"
#include "iir.h"

#if  TCFG_PC_ENABLE

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[UAC]"
#include "log.h"
/* #include "uart.h" */


EFFECT_OBJ *usb_src_obj = NULL;
uac_sync uac_spk_sync;
u32 uac_spk_all;
u32 uac_spk_cnt;


EFFECT_OBJ *uac_spk_percent(u32 *p_percent)
{
    if (0 == uac_spk_cnt) {
        return NULL;
    }
    if (NULL == usb_src_obj) {
        return NULL;
    }
    sound_in_obj *p_src_si = usb_src_obj->p_si;
    if (NULL == p_src_si) {
        return NULL;
    }
    SRC_STUCT_API *p_ops =  p_src_si->ops;
    if (NULL == p_ops) {
        return NULL;
    }
    *p_percent = uac_spk_all / uac_spk_cnt;
    uac_spk_all = 0;
    uac_spk_cnt = 0;
    return usb_src_obj;

}

cbuffer_t cbuf_src_o AT(.speaker_data);
u8 obuf_src_o[1024]  AT(.speaker_data);

void usb_slave_sound_open(sound_out_obj *p_sound, u32 sr)
{
    sound_out_obj *p_curr_sound = 0;
    sound_out_obj *p_next_sound = 0;

    uac_sync_init(&uac_spk_sync, sr);

    if (0 != sr) {
        p_curr_sound = p_sound;
        void *cbuf_o = p_curr_sound->p_obuf;
#if (defined(HAS_HW_SRC_EN) || defined(HAS_SW_SRC_EN))
        u32 dac_sr = dac_sr_read();
        p_curr_sound = link_src_sound(p_curr_sound, cbuf_o, (void **)&usb_src_obj, sr, dac_sr, (void *)GET_SRC_OPS());
#endif

        regist_dac_channel(p_sound, NULL);//注册到DAC;
        p_sound->enable |=  B_DEC_RUN_EN | B_DEC_FIRST;
    }
}

void usb_slave_sound_close(sound_out_obj *p_sound)
{
    log_info("usb slave sound off\n");
    p_sound->enable &= ~B_DEC_RUN_EN;
    unregist_dac_channel(p_sound);
    usb_src_obj = NULL;
    if (NULL != p_sound->effect) {
#if (defined(HAS_HW_SRC_EN) || defined(HAS_SW_SRC_EN))
        src_reless(&p_sound->effect);
#endif
    } else {
        log_info("usb slave sound effect null\n");
    }
}

#if USB_DEVICE_CLASS_CONFIG & MIC_CLASS
sound_out_obj usb_mic_sound;
u8 obuf_usb_mic_o[1024];
cbuffer_t cbuf_usb_mic_o;
void usb_mic_init(void)
{
    log_info("usb mic init\n");
    u32 err;
    memset(&usb_mic_sound, 0, sizeof(usb_mic_sound));

    cbuf_init(&cbuf_usb_mic_o, &obuf_usb_mic_o[0], sizeof(obuf_usb_mic_o));
    usb_mic_sound.p_obuf = &cbuf_usb_mic_o;

    //16k 采样,单声道
    err = audio_adc_init_api(MIC_AUDIO_RATE, ADC_MIC, audio_adc_mic_input_port);  //PA13 -> mic
    regist_audio_adc_channel(&usb_mic_sound, NULL); //注册到ADC;
    audio_adc_enable(14);
    usb_mic_sound.enable |= B_DEC_RUN_EN;
}
void usb_mic_uninit()
{
    log_info("usb slave mic off \n");
    usb_mic_sound.enable &= ~B_DEC_RUN_EN;
    audio_adc_off_api();
    unregist_audio_adc_channel(&usb_mic_sound); //卸载ADC
    audio_adc_disable();
}
#endif

#endif
