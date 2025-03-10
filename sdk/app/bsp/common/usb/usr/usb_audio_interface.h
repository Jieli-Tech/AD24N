#ifndef __USB_AUDIO_INTERFACE_H__
#define __USB_AUDIO_INTERFACE_H__


#include "typedef.h"
#include "src.h"
#include "src_api.h"
#include "dac_api.h"
#include "sound_effect_api.h"
#include "circular_buf.h"
#include "app_config.h"
#include "audio_adc.h"
#include "uac_sync.h"


void uac_1s_sync(void);
void usb_slave_sound_close(sound_out_obj *p_sound);
void usb_slave_sound_open(sound_out_obj *p_sound, u32 sr);
EFFECT_OBJ *uac_spk_percent(u32 *p_percent);

#if  TCFG_PC_ENABLE
#include "usb_common_def.h"
#if USB_DEVICE_CLASS_CONFIG & MIC_CLASS
extern cbuffer_t cbuf_usb_mic_o;
void usb_mic_init(void);
void usb_mic_uninit(void);
#endif
#endif

#endif
