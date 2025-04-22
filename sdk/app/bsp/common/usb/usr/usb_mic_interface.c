#include "audio_adc_cpu.h"
#include "audio_adc.h"
#include "audio_analog.h"
#include "usb_mic_interface.h"
#include "usb_common_def.h"
#include "uac_sync.h"

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[UMIC]"
#include "log.h"
#include "uart.h"

#if USB_DEVICE_CLASS_CONFIG & MIC_CLASS

extern const bool config_usbslave_ctl_mic;
sound_out_obj usb_mic_sound SEC(.uac_var);
u16 mic_out_obuf[AUDIO_ADC_PACKET_SIZE * 2] SEC(.uac_var);
cbuffer_t mic_out_cbuf SEC(.uac_var);

cbuffer_t mic_run_cbuf SEC(.uac_var);
u16 mic_run_obuf[AUDIO_ADC_PACKET_SIZE * 4] SEC(.uac_var);

uac_mic_read uac_read;
uac_sync uac_mic_sync SEC(.uac_var);

u32 uac_mic_stream_size()
{
    if (NULL != uac_read.read_sound) {
        return cbuf_get_data_size(uac_read.read_sound->p_obuf);
    }
    return 50;
}
u32 uac_mic_stream_buf_length()
{
    if (NULL != uac_read.read_sound) {
        return cbuf_get_space(uac_read.read_sound->p_obuf);
    }
    return 100;
}

void usb_slave_mic_open(u32 sr, u32 frame_len, u32 ch)
{
    log_info("usb mic init\n");
    u32 err = 0;
    memset(&usb_mic_sound, 0, sizeof(usb_mic_sound));

    stream_sound_init(&usb_mic_sound, NULL);

    cbuf_init(&mic_out_cbuf, &mic_out_obuf[0], sizeof(mic_out_obuf));
    /*usb_mic_sound.p_obuf = &mic_out_cbuf;*/
    sound_out_init(&usb_mic_sound, (void *)&mic_out_cbuf, 0);

    cbuf_init(&mic_run_cbuf, &mic_run_obuf[0], sizeof(mic_run_obuf));

    sound_out_obj *p_curr_sound = 0;
    p_curr_sound = &usb_mic_sound;

#if TCFG_MIC_SRC_ENABLE
    p_curr_sound = link_src_sound(p_curr_sound, &mic_run_cbuf, (void **)(&uac_read.p_src), sr, sr, (void *)GET_SRC_OPS());

    uac_sync_init(&uac_mic_sync, sr);
#endif

    uac_read.read_sound =  p_curr_sound;
    if (config_usbslave_ctl_mic) {//打开MIC的时候会有杂声,放在usb初始化之前会好很多
        log_info("usmo ---- 006");
        err = audio_adc_init_api(sr, ADC_MIC, audio_adc_mic_input_port);
#if defined(TCFG_MIC_CAPLESS) && (TCFG_MIC_CAPLESS == 1)
        audio_adc_trim();
#endif
    }
    if (&usb_mic_sound == p_curr_sound) {
        regist_audio_adc_channel(&usb_mic_sound, (void *) NULL);
    } else {
        regist_audio_adc_channel(&usb_mic_sound, (void *) kick_sound);
    }
    if (0 == err) {
        if (config_usbslave_ctl_mic) {
            audio_adc_enable(14);
        }
        p_curr_sound->enable |= B_DEC_RUN_EN | B_DEC_FIRST;
        usb_mic_sound.enable |= B_DEC_RUN_EN;
    } else {
        log_info("err : 0x%x", err);
    }
}


void usb_slave_mic_close(void)
{
    if (usb_mic_sound.enable & B_DEC_RUN_EN) {
        usb_mic_sound.enable &= ~B_DEC_RUN_EN;
        unregist_audio_adc_channel(&usb_mic_sound);
        /*unregist_stream_sound();*/
        stream_sound_uninit();
        if (NULL != uac_read.p_src) {
            src_reless((void **)(&uac_read.p_src));
        }
        memset(&usb_mic_sound, 0, sizeof(usb_mic_sound));
    }
    if (config_usbslave_ctl_mic) {
        audio_adc_off_api();
        audio_adc_disable();
    }
}

static u32 uac_mic_all;
static u32 uac_mic_cnt;
static u32 uac_mic_cnt_last;
static u32 last_percent;
EFFECT_OBJ *uac_mic_percent(u32 *p_percent)
{
    if (uac_mic_cnt == 0) {
        return NULL;
    }
    if ((uac_mic_cnt_last + 0) == uac_mic_cnt) {
        return NULL;
    }
    uac_mic_cnt_last = uac_mic_cnt;
    *p_percent = uac_mic_all / uac_mic_cnt;
    uac_mic_all = 0;
    uac_mic_cnt = 0;
    /*log_info("percent = %d\n",*p_percent);*/
    if (NULL == uac_read.p_src) {
        return NULL;
    }
    sound_in_obj *p_src_si = uac_read.p_src->p_si;
    if (NULL == p_src_si) {
        return 0;
    }
    SRC_STUCT_API *p_ops =  p_src_si->ops;
    if (NULL == p_ops) {
        return 0;
    }
    return uac_read.p_src;

}

int usb_slave_mic_read(u8 *buf, u32 len)//目前只考虑到单声道
{
    int rlen = 0;

    if (uac_read.read_sound) {
        sound_out_obj *psound = uac_read.read_sound;
        if (0 == (psound->enable & B_DEC_RUN_EN)) {
            psound->enable &= ~B_DEC_RUN_EN;
            goto __no_read;
        }

        if (psound->enable & B_DEC_FIRST) {
            if (cbuf_get_data_size(psound->p_obuf) < (cbuf_get_space(psound->p_obuf) / 2)) {
                goto __no_read;
            } else {
                psound->enable &= ~B_DEC_FIRST;
            }
        }
        rlen = sound_input(uac_read.read_sound, buf, len);
        u32 uac_mic_data = uac_mic_stream_size();
        u32 uac_mic_size = uac_mic_stream_buf_length();

        if (0 != uac_mic_size) {
            u32 percent = (uac_mic_data * 100) / uac_mic_size;
            uac_mic_all += percent;
            uac_mic_cnt++;
        }
    } else {
        memset(buf, 0, len);
        return (len);
    }
__no_read:
    if (rlen != len) {
        /*log_char('.');*/
        memset(buf + rlen, 0, len - rlen);
        rlen = len;
    }

    return len;
}


#endif

