#ifndef __AUDIO_TRIM_CPU_H__
#define __AUDIO_TRIM_CPU_H__

#include "typedef.h"

typedef struct __autrim_para {
    u32(*fun)(u32);
    u16 need_mv;
    u16 max_limit_mv;
    u8 choose_larger;
    u8 max_gear;
} autrim_para;

typedef struct __autrim_res {
    u16 res_mv;
    u8 gear;
} autrim_res;

extern autrim_res g_adda_high_voltage_vbg; //存放高电压模式下AUDIO_VBG的校准结果
extern const u8 config_adda_low_voltage_vbg;
void audio_adda_trim_analog_open(u8 vcm_level);
void audio_adda_trim_analog_close();
u32 aud_vbg_trim(autrim_para *p_autrim_para, autrim_res *p_autrim_res);
u32 audio_vbg_adjust(u32 level);
u32 audio_vbg_trim(u32 need_mv, autrim_res *p_autrim_res);

typedef struct __vbtrim_para {
    u32(*fun)(u32);
    u16 need_ad;
    // u16 max_limit_mv;
    u8 max_gear;
} vbtrim_para;

typedef struct __vbtrim_res {
    u16 res_ad;
    u8 gear;
} vbtrim_res;
u32 audio_apa_vb17_trim(vbtrim_res *p_vbtrim_res);


#endif
