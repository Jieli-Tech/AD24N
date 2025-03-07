#include "howling_pitchshifter_api.h"
#include "decoder_api.h"
#include "config.h"
#include "sound_effect_api.h"
#include "app_modules.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

#if defined(PITCHSHIFT_HOWLING_EN) && (PITCHSHIFT_HOWLING_EN)


/* sdk需要添加这个定义，代表滤波器阶数参数: 配置范围为2<=>8，它的大小跟运算量成正相关 */
const int FRESHIFT_SPEED_MODE_QUALITY = 2;
const int FRESHIFT_USE_LONG_FILTER = 1;
const int FRESHIFT_USE_BAND_RESTICT = 1;
u32 howling_work_buf[(1980 + 3) / 4] AT(.howling_data);


static void *pitchshift_howling_api(void *obuf, u32 sr, void **ppsound)
{
    HOWLING_PITCHSHIFT_PARM phparm  = {0};
    phparm.ps_parm                  = -200; //等比移频，建议范围：-350到350，归一化系数为8192
    phparm.fs_parm                  = -10;  //线性移频，单位Hz，移频方式要选择FS，移动多少Hz就配多少Hz，向高频移取正数，向低频移取负数，配置的采样率要和声音输入源的采样率一致
    phparm.effect_v                 = EFFECT_HOWLING_PS; //选择需要的移频方式

    HOWLING_PITCHSHIFT_FUNC_API *ops;
    ops = (HOWLING_PITCHSHIFT_FUNC_API *)get_howling_ps_func_api(); //接口获取
    u32 buf_len = ops->need_buf(phparm.effect_v);                   //运算空间获取
    log_info("howling work_buf_len %d\n", buf_len);
    if (sizeof(howling_work_buf) < buf_len) {
        log_error("howing work buf less %d, need len %d", sizeof(howling_work_buf), buf_len);
        return NULL;
    }

    return howling_phy(obuf, &howling_work_buf[0], &phparm, sr, ppsound);
}

void *link_pitchshift_howling_sound(void *p_sound_out, void *p_dac_cbuf, void **pp_effect, u32 sr)
{
    sound_out_obj *p_next_sound = 0;
    sound_out_obj *p_curr_sound = p_sound_out;
    p_curr_sound->effect = pitchshift_howling_api(p_curr_sound->p_obuf, sr, (void **)&p_next_sound);
    if (NULL != p_curr_sound->effect) {
        if (NULL != pp_effect) {
            *pp_effect = p_curr_sound->effect;
        }
        p_curr_sound->enable |= B_DEC_EFFECT;
        p_curr_sound = p_next_sound;
        p_curr_sound->p_obuf = p_dac_cbuf;
        log_info("pitchshift howling init succ\n");
    } else {
        log_info("pitchshift howling init fail\n");
    }
    return p_curr_sound;
}

/* 动态更新线性移频参数 */
void update_howling_parm_fs_api(u32 sr, s16 new_fs)
{
    update_howling_parm_fs(&howling_work_buf[0], sr, new_fs);
}
#endif

