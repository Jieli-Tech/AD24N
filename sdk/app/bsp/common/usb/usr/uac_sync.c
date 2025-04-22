
#include "uac_sync.h"
#include "app_config.h"
#include "sound_effect_api.h"
#include "usb_audio_interface.h"
#include "usb_mic_interface.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[SYNC]"
#include "log.h"

#if 0
#define uac_sync_log(a)  putchar(a)
#else
#define uac_sync_log(a)
#endif

#define SYNC_RATIO 1
#define THOUSAND_RATIO 10
#define HUNDRED_RATIO 1

void uac_sync_init(uac_sync *sync, s32 sr)
{
    if (NULL == sync) {
        return;
    }
    memset(sync, 0, sizeof(uac_sync));
    sync->pe5_step  = 20;
    sync->pe5_dec  = 10;
    sync->pe_change_cnt = 0;
    sync->last_pe = (u8) - 1;
    sync->x_step = sr * 5 / 10000;
    sync->baseline_pe = (u8) - 1;
    /* sync->sr_curr = 0; */
    /* sync->pe_cnt  = 0; */
    /* sync->pe5_cnt  = 0; */
    /* sync->pe_inc_data = 0; */
    /* sync->pe_sub_data = 0; */
    /* sync->last_sr = 0; */

    initLowPassFilter(&sync->f_percent, 85);

}

void uac_1s_sync(void)
{
    return;
}

static void uac_inc_sync_pe_reset(uac_sync *sync)
{
    sync->pe5_step += 40;
    sync->pe5_cnt = sync->pe5_step;
    /* sync->pe_inc_data = 0; */
    /* sync->pe_sub_data = 0; */
    /* sync->pe_cnt = 0; */
    sync->last_pe = (u8) - 1;
}

#ifdef SYNC_VOICE_LIGHT//语音灯--voice_light

void uac_inc_sync_one(EFFECT_OBJ *e_obj, u32 percent, uac_sync *sync)
{
    sound_in_obj *p_src_si = e_obj->p_si;
    SRC_STUCT_API *p_ops =  p_src_si->ops;
    char c = 0;     //大调
    char d = 0;     //微调
    s32 step = 0;
    u32 temp_percent = percent;
    /* putchar(percent); */
    percent = applyLowPassFilter(&sync->f_percent, percent);

    if ((u8) - 1 == sync->baseline_pe) {
        sync->baseline_pe = percent;	//基线赋予初值
        sync->last_pe = percent;
        return;
    }

    uac_sync_log(0x55);
    /*uac_sync_log(0xaa);*/
    uac_sync_log(0x77);
    uac_sync_log(percent);
    uac_sync_log(temp_percent);

    void get_per_src_obuf();
    /* get_per_src_obuf(); */
    bool flag_out_range = 1;
    /* 范围以外，大调 */
    if (percent > 58 * SYNC_RATIO) {
        if (0 == sync->pe5_cnt) {
            if (0 != sync->pe_cnt) {
                step = ((s16)sync->pe_inc_data * 8) / sync->pe_cnt;
            }
            if (0 == sync->pe_sub_data) {
                if (0 == step) {
                    /*step = 1;*/
                    step = 2;
                    /*step = 4;*/
                } else {
                    step = step * sync->x_step;
                }
            }
            uac_inc_sync_pe_reset(sync);
            c = 100 + step;
        }
    } else if (percent < 42 * SYNC_RATIO) {
        if (0 == sync->pe5_cnt) {
            if (0 != sync->pe_cnt) {
                step = (((s16)sync->pe_sub_data * 8) / sync->pe_cnt);
            }
            if (0 == sync->pe_inc_data) {
                if (0 == step) {
                    /*step = -1;*/
                    /*step = -5;//-4或者-5*/
                    step = -4;//-4或者-5
                } else {
                    step = 0 - (step * sync->x_step);
                }
            }
            uac_inc_sync_pe_reset(sync);
            c = 100 + step;
        }
    } else {
        /* 区间范围以内 */
        flag_out_range = 0;
        sync->pe5_step = 0;
    }

    if ((sync->last_pe < 100 * SYNC_RATIO) && (percent == sync->last_pe)) {
        if (2 < sync->pe5_dec) {
            sync->pe5_dec -= 2;
        }
    } else {
        sync->pe5_dec = 10;
    }

    if (0 != sync->pe5_cnt) {
        u32 tmp = (sync->pe5_cnt < sync->pe5_dec ? sync->pe5_cnt : sync->pe5_dec);
        sync->pe5_cnt -= tmp;
    }


    if (sync->last_pe <= 100 * SYNC_RATIO) {
        if (percent < sync->last_pe) {  //降
            if (sync->baseline_pe < sync->last_pe) {    //反转
                sync->pe_inc_data = 0;
                sync->pe_cnt = sync->pe_change_cnt;
                sync->pe_change_cnt = 0;
                sync->baseline_pe = sync->last_pe;
            }
            sync->pe_sub_data += (sync->last_pe - percent) / SYNC_RATIO;
        } else if (percent > sync->last_pe) {   //升
            if (sync->baseline_pe > sync->last_pe) {    //反转
                sync->pe_sub_data = 0;
                sync->pe_cnt = sync->pe_change_cnt;
                sync->pe_change_cnt = 0;
                sync->baseline_pe = percent;
            }
            sync->pe_inc_data += (percent - sync->last_pe) / SYNC_RATIO;
        }
    }
    sync->pe_cnt++;
    sync->pe_change_cnt++;

    /* 若是范围以内，微调 */
    if ((sync->last_pe <= 100 * SYNC_RATIO) && (0 == flag_out_range)) {
        if (sync->pe_sub_data > 1) {
            step = (sync->pe_sub_data * 8) / sync->pe_cnt;
            if (step > 0) {
                step = (step * sync->x_step);
            } else {
                u32 tmp = sync->baseline_pe - percent;
                if (tmp > 10) {
                    step =  tmp / 3 + 1;
                } else {
                    step = 1;
                }
            }
            step =  0 - step;
            d = 100 + step;
            if (percent > 10 * SYNC_RATIO) {
                sync->pe_sub_data = 0;
                sync->pe_cnt = 0;
            }
        } else if (sync->pe_inc_data > 1) {
            step = (sync->pe_inc_data * 8) / sync->pe_cnt;
            if (step > 0) {
                step = (step * sync->x_step);
            } else {        //数据缓慢增长
                u32 tmp = percent - sync->baseline_pe;
                if (tmp > 10) {     //数据持续增长的情况下，与基线相差很大
                    step =  tmp / 3 + 1;
                } else {
                    step = 1;
                }
            }
            d = 100 + step;
            if (percent < 90 * SYNC_RATIO) {
                sync->pe_inc_data = 0;
                sync->pe_cnt = 0;
            }
        }

    }


    if (0 != step) {
        sync->uac_sync_parm = p_ops->config(
                                  p_src_si->p_dbuf,
                                  SRC_CMD_INSR_INC_SET,
                                  (void *)step
                              );
        /* log_info("S %d %d\n",step, insmaple_inc); */
    }
    /* uac_sync_log(sync->baseline_pe); */
    if (c == 0) {
        c = 100;        //大调
    }
    uac_sync_log(c);
    if (d == 0) {
        d = 100;        //小调
    }
    uac_sync_log(d);

    sync->last_pe = percent;
}

#else//玩具--voice_toy

void uac_inc_sync_one(EFFECT_OBJ *e_obj, u32 percent, uac_sync *sync)
{
    sound_in_obj *p_src_si = e_obj->p_si;
    SRC_STUCT_API *p_ops =  p_src_si->ops;
    char c = 0;     //大调
    char d = 0;     //微调
    s32 step = 0;
    percent = applyLowPassFilter(&sync->f_percent, percent);

    if ((u8) - 1 == sync->baseline_pe) {
        sync->baseline_pe = percent;	//基线赋予初值
        sync->last_pe = percent;
        return;
    }

    uac_sync_log(0x55);
    uac_sync_log(0xaa);
    uac_sync_log(percent);

    void get_per_src_obuf();
    /* get_per_src_obuf(); */
    bool flag_out_range = 1;
    /* 范围以外，大调 */
    if (percent > 58 * SYNC_RATIO) {
        if (0 == sync->pe5_cnt) {
            if (0 != sync->pe_cnt) {
                step = ((s16)sync->pe_inc_data * 8) / sync->pe_cnt;
            }
            if (0 == sync->pe_sub_data) {
                if (0 == step) {
                    step = 1;
                } else {
                    step = step * sync->x_step;
                }
            }
            uac_inc_sync_pe_reset(sync);
            c = 100 + step;
        }
    } else if (percent < 42 * SYNC_RATIO) {
        if (0 == sync->pe5_cnt) {
            if (0 != sync->pe_cnt) {
                step = (((s16)sync->pe_sub_data * 8) / sync->pe_cnt);
            }
            if (0 == sync->pe_inc_data) {
                if (0 == step) {
                    step = -1;
                } else {
                    step = 0 - (step * sync->x_step);
                }
            }
            uac_inc_sync_pe_reset(sync);
            c = 100 + step;
        }
    } else {
        /* 区间范围以内 */
        flag_out_range = 0;
        sync->pe5_step = 0;
    }

    if ((sync->last_pe < 100 * SYNC_RATIO) && (percent == sync->last_pe)) {
        if (2 < sync->pe5_dec) {
            sync->pe5_dec -= 2;
        }
    } else {
        sync->pe5_dec = 10;
    }

    if (0 != sync->pe5_cnt) {
        u32 tmp = (sync->pe5_cnt < sync->pe5_dec ? sync->pe5_cnt : sync->pe5_dec);
        sync->pe5_cnt -= tmp;
    }


    if (sync->last_pe <= 100 * SYNC_RATIO) {
        if (percent < sync->last_pe) {  //降
            if (sync->baseline_pe < sync->last_pe) {    //反转
                sync->pe_inc_data = 0;
                sync->pe_cnt = sync->pe_change_cnt;
                sync->pe_change_cnt = 0;
                sync->baseline_pe = sync->last_pe;
            }
            sync->pe_sub_data += (sync->last_pe - percent) / SYNC_RATIO;
        } else if (percent > sync->last_pe) {   //升
            if (sync->baseline_pe > sync->last_pe) {    //反转
                sync->pe_sub_data = 0;
                sync->pe_cnt = sync->pe_change_cnt;
                sync->pe_change_cnt = 0;
                sync->baseline_pe = percent;
            }
            sync->pe_inc_data += (percent - sync->last_pe) / SYNC_RATIO;
        }
    }
    sync->pe_cnt++;
    sync->pe_change_cnt++;

    /* 若是范围以内，微调 */
    if ((sync->last_pe <= 100 * SYNC_RATIO) && (0 == flag_out_range)) {
        if (sync->pe_sub_data > 1) {
            step = (sync->pe_sub_data * 8) / sync->pe_cnt;
            if (step > 0) {
                step = (step * sync->x_step);
            } else {
                u32 tmp = sync->baseline_pe - percent;
                if (tmp > 10) {
                    step =  tmp / 3 + 1;
                } else {
                    step = 1;
                }
            }
            step =  0 - step;
            d = 100 + step;
            if (percent > 10 * SYNC_RATIO) {
                sync->pe_sub_data = 0;
                sync->pe_cnt = 0;
            }
        } else if (sync->pe_inc_data > 1) {
            step = (sync->pe_inc_data * 8) / sync->pe_cnt;
            if (step > 0) {
                step = (step * sync->x_step);
            } else {        //数据缓慢增长
                u32 tmp = percent - sync->baseline_pe;
                if (tmp > 10) {     //数据持续增长的情况下，与基线相差很大
                    step =  tmp / 3 + 1;
                } else {
                    step = 1;
                }
            }
            d = 100 + step;
            if (percent < 90 * SYNC_RATIO) {
                sync->pe_inc_data = 0;
                sync->pe_cnt = 0;
            }
        }

    }


    if (0 != step) {
        sync->uac_sync_parm = p_ops->config(
                                  p_src_si->p_dbuf,
                                  SRC_CMD_INSR_INC_SET,
                                  (void *)step
                              );
        /* log_info("S %d %d\n",step, insmaple_inc); */
    }
    /* uac_sync_log(sync->baseline_pe); */
    if (c == 0) {
        c = 100;        //大调
    }
    uac_sync_log(c);
    if (d == 0) {
        d = 100;        //小调
    }
    uac_sync_log(d);

    sync->last_pe = percent;
}
#endif

extern uac_sync uac_spk_sync;
extern uac_sync uac_mic_sync;
void uac_inc_sync(void)
{
    EFFECT_OBJ *e_obj = NULL;
    u32 percent;

#if (TCFG_PC_ENABLE && (USB_DEVICE_CLASS_CONFIG & MIC_CLASS))
#if TCFG_MIC_SRC_ENABLE
    e_obj = uac_mic_percent(&percent);
    if (NULL != e_obj) {
        uac_inc_sync_one(e_obj, percent, &uac_mic_sync);
    }
#endif
#endif

#if (TCFG_PC_ENABLE && (USB_DEVICE_CLASS_CONFIG & SPEAKER_CLASS))
#if TCFG_SPK_SRC_ENABLE
    e_obj = uac_spk_percent(&percent);
    if (NULL != e_obj) {
        uac_inc_sync_one(e_obj, percent, &uac_spk_sync);
    }
#endif
#endif
}

