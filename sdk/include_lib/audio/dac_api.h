#ifndef __DAC_API_H__
#define __DAC_API_H__
#include "typedef.h"
#include "dac.h"
//#include "typedef.h"

typedef enum {
    SR_NORMAL = 1,
    SR_CONSTANT,
} SR_WORKMODE;

#ifdef CPU_SH57
#define DAC_PACKET_SIZE         (32*2)
// #define DAC_DECODER_KICK_SIZE   (DAC_PACKET_SIZE * 4 * 4)
#define DAC_DECODER_BUF_SIZE    (DAC_PACKET_SIZE * 4 * 5)
#else
#define DAC_PACKET_SIZE         (32*8)
// #define DAC_DECODER_KICK_SIZE   (DAC_PACKET_SIZE * 4)
#define DAC_DECODER_BUF_SIZE    (DAC_PACKET_SIZE * 5)
#endif



bool dac_mute(bool mute);
void dac_mode_init(u16 vol, void *phy_vol_set_func);
void dac_init_api(u32 sr, bool delay_flag);
void dac_sr_api(u32 sr);
void dac_off_api(void);
// void dac_sr_api(u32 sr);
u32 fill_dac_fill(u8 *buf, u32 len, AUDIO_TYPE type);
u8 dac_vol(char set, u8 vol);
void dac_usb_vol(u8 vol_r, u8 vol_l);
bool regist_dac_channel(void *sound, void *kick);
bool unregist_dac_channel(void *dec_hld);
bool dac_cbuff_active(void *sound_hld);

/*---------DAC FADE IN & OUT Configuration---------------*/
#ifdef D_MBOX_SDK
#define 	DAC_FADE_ENABLE		1
#else
#define 	DAC_FADE_ENABLE		0
#endif
#if DAC_FADE_ENABLE
void dac_fade(void);
void dac_fade_in(void);
void dac_fade_out(u32 delay);
#define dac_fade_api        dac_fade
#define dac_fade_in_api     dac_fade_in
#define dac_fade_out_api    dac_fade_out

#else
#define dac_fade_api()
#define dac_fade_in_api()
#define dac_fade_out_api(...)

#endif
//apa & audio dac初始化；后续加入audio_link可能发生变化
#define auout_mode_init  audio_dac_para_init
#define auout_init       audio_dac_init
#define auout_sr_api     audio_dac_sr_api
#define auout_off_api     audio_dac_off_api


#endif


