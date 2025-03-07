#include "typedef.h"
#include "clock.h"
#include "vm_api.h"
#include "adc_drv.h"
#include "audio_cpu.h"
#include "audio_trim_cpu.h"
#include "errno-base.h"

/* #define LOG_TAG_CONST       OFF */
#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[audio_power_trim]"
#include "log.h"
/*****************AUDIO VBG 配置************************************************/
// 高压模式，config_adda_low_voltage_mode = 0时
#define AUD_VBG_1200mV         0
#define AUD_VBG_1300mV         1
#define AUD_VBG_1400mV         2
#define AUD_VBG_1500mV         3
#define AUD_VBG_MAX_NUMBER     8
autrim_res g_adda_high_voltage_vbg; //存放高电压模式下AUDIO_VBG的校准结果
// 低压模式，config_adda_low_voltage_mode = 1时
#define LOW_VOL_VBG_751mV   0b00
#define LOW_VOL_VBG_782mV   0b01
#define LOW_VOL_VBG_810mV   0b10
#define LOW_VOL_VBG_838mV   0b11
const u8 config_adda_low_voltage_vbg =  LOW_VOL_VBG_810mV; //存放低压模式下AUDIO_VBG挡位
/*****************AUDIO VBG 配置 END********************************************/


u32 audio_vbg_adjust(u32 level)
{
    SFR(JL_ADDA->ADDA_CON0, 11, 3, level);	//AUDIO_VBG_VSEL_11v[4:3], coarse-step, 0: 1.2v(x)	1: 1.3v(√)	2: 1.4v(x)	3: 1.5v(√)
    mdelay(5); //delay 5ms
    u32 aud_vbg = adc_get_voltage_blocking(ADC_CH_AUDIO_VCM);
    /* log_info("level %d, aud_vbg %d\n",level, aud_vbg); */
    return aud_vbg;
}

u32 audio_vbg_trim(u32 need_mv, autrim_res *p_autrim_res)
{
    autrim_para audio_trim_para; //校准前传入的参数
    memset((u8 *)&audio_trim_para, 0, sizeof(audio_trim_para));

    audio_trim_para.fun = audio_vbg_adjust;
    audio_trim_para.need_mv = need_mv;
    audio_trim_para.choose_larger = 0;
    audio_trim_para.max_gear = AUD_VBG_MAX_NUMBER - 1;
    audio_trim_para.max_limit_mv = 5000;
    /* log_info("need_mv_should_be %d\n", audio_trim_para.need_mv); */
    u32 trim_ret = aud_vbg_trim(&audio_trim_para, p_autrim_res);
    /* log_info("res_mv %d, %d\n", p_autrim_res->res_mv, p_autrim_res->gear); */

    return trim_ret;

}


void audio_voltage_trim(u32 iovdd_mv)
{
    if (1 == config_adda_low_voltage_mode) {
        /* 低压不需要trim Audio_VBG */
        return;
    }

    /* autrim_res g_adda_high_voltage_vbg; //校准结果 */
    memset((u8 *)&g_adda_high_voltage_vbg, 0, sizeof(g_adda_high_voltage_vbg));
    u32 rlen = 0;
    rlen = vm_read(VM_INDEX_AUDIO_VBG_TRIM, &g_adda_high_voltage_vbg.gear, sizeof(g_adda_high_voltage_vbg.gear));
    if (rlen == sizeof(g_adda_high_voltage_vbg.gear)) {
        /* log_info("read_trim_value\n"); */
        /* SFR(JL_ADDA->ADDA_CON0, 11, 3, low3gear); */
        /* mdelay(5);// delay 5ms */
        /* return; */
        goto __vb17_trim_check;
    }

    u32 need_mv = (iovdd_mv - 300) / 2; //需要的是小于或等于该值的电压

    u32 vcm_level;
    if (need_mv < 1200) {
        log_error("need audio vbg too low : %d mV, check iovdd_vol~", need_mv);
        vcm_level = AUD_VBG_1200mV;
    } else if (need_mv < 1300) {
        need_mv = 1200;
        vcm_level = AUD_VBG_1200mV;
    } else if (need_mv < 1400) {
        need_mv = 1300;
        vcm_level = AUD_VBG_1300mV;
    } else if (need_mv < 1500) {
        need_mv = 1400;
        vcm_level = AUD_VBG_1400mV;
    } else {
        need_mv = 1500;
        vcm_level = AUD_VBG_1500mV;
    }

    audio_adda_trim_analog_open(vcm_level);

    u32 res = audio_vbg_trim(need_mv, &g_adda_high_voltage_vbg);
    g_adda_high_voltage_vbg.gear &= ~(0x3 << 3);
    g_adda_high_voltage_vbg.gear |= ((vcm_level & 0x3) << 3);
    audio_adda_trim_analog_close();


    if (0 == res) {
        vm_write(VM_INDEX_AUDIO_VBG_TRIM, &g_adda_high_voltage_vbg.gear, sizeof(g_adda_high_voltage_vbg.gear));
    }

__vb17_trim_check:
#if 0 //测试需要再打开
    vbtrim_res apa_vb17; //存放5V供电VB17的校准结果
    res = audio_apa_vb17_trim(&apa_vb17);
#endif
    return;
}

