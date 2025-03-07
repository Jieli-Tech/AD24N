#include "app_power_mg.h"
#include "adc_drv.h"
#include "key.h"
#include "msg.h"
#include "efuse.h"
#include "clock.h"
#include "asm/power_interface.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

#if LVD_WARNING_FOR_LOW_POWER
static u16 lvd_warning_voltage;//低电检测电压,一般设置为比lvd电压大200mV
#endif

static void lvd_warning_init(void)
{
    u16 lvd_voltage = get_lvd_vol();
    lvd_warning_voltage = lvd_voltage + 200;
    /* log_info("lvd_warning_voltage : %d real : %d\n", lvd_warning_voltage, lvd_voltage); */
}

static volatile u8 powerup_kick_start_cnt = 0;
static void app_power_powerup_kick_start(void)
{
    if (powerup_kick_start_cnt++ < 10) {
        adc_kick_start(app_power_powerup_kick_start);
        return;
    }
    log_info("vbat value:%d \n", app_power_get_vbat());
}

void app_power_init(void)
{
    low_power_warning_init();
    adc_add_sample_ch(ADC_CH_PMU_VBAT);
    adc_kick_start(app_power_powerup_kick_start);
    while (powerup_kick_start_cnt < 10);
}

u32 app_power_get_vbat(void)
{
    u32 vol = adc_get_voltage(ADC_CH_PMU_VBAT);
    if (-1 != vol) {
        vol *= 4;
    }
    return vol;
}

void app_power_scan(void)
{
    static u16 low_power_cnt = 0;
    u32 vol = adc_get_voltage(ADC_CH_PMU_VBAT);

    if (-1 != vol) {
        vol = vol * 4;
        if (vol <= LOW_POWER_VOL) {
            low_power_cnt++;
            if (low_power_cnt == 10) {
                log_error(LOW_POWER_LOG);
                post_msg(1, MSG_LOW_POWER);
            }
        } else {
            low_power_cnt = 0;
        }
    }

#if (!(KEY_AD_EN || KEY_MATRIX_EN))
    adc_kick_start(NULL);
#endif
}
