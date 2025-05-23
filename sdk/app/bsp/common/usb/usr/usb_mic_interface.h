#ifndef __USB_MIC_INTERFACE_H__
#define __USB_MIC_INTERFACE_H__


#include "sound_effect_api.h"
#include "src.h"
#include "src_api.h"
#include "circular_buf.h"
#include "app_config.h"
//#include "audio_adc.h"

typedef struct __uac_mic_info {
    u32 sr;
    u32 frame_len;
    u32 ch;
} uac_mic_info;

typedef struct __uac_mic_read {
    sound_out_obj *read_sound;
    EFFECT_OBJ *p_src;
    void (*pkick)(void *, void *);
    uac_mic_info self;
    void *source;
} uac_mic_read;

void usb_slave_mic_open(u32 sr, u32 frame_len, u32 ch);
EFFECT_OBJ *uac_mic_percent(u32 *p_percent);
void usb_slave_mic_close(void);
int usb_slave_mic_read(u8 *buf, u32 len);
#endif
