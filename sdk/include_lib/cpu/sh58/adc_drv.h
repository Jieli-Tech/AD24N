#ifndef __SIMPLE_SARADC_H_
#define __SIMPLE_SARADC_H_

#include "typedef.h"
// #include "app_config.h"
#define ADC_MASK_CHANNEL_SEL    (0x3f)
#define ADC_MUX_IO              (0x0 << 4)  //普通IO
#define ADC_MUX_AN              (0x1 << 4)  //内部模拟电源通道
#define ADC_AUDIO_AN            (0x2 << 4)  //内部模拟 AUDIO 通道
#define ADC_CLASSD_AN           (0x3 << 4)  //内部模拟 CLASSD 通道
#define ADC_IO_CAN_NOT_USE      (0x1 << 6)  //ADC通道IO繁忙

#define ADC_AUDIO_SUB           (0x1 << 3)



#define ADC_SFR_AUDIO        (0x01)
#define ADC_SFR_PMU          (0x02)
#define ADC_SFR_LRC200K      (0x03)
#define ADC_SFR_SYSPLL       (0x05)
#define ADC_SFR_WAT          (0x06)
#define ADC_SFR_CLASSD       (0x07)

#define ADC_CON_DONE_PND      31
#define ADC_CON_DONE_PND_CLR  30
#define ADC_CON_DONE_PND_IE   29
#define ADC_CON_RESERVED      24 //bit28~bit24
#define ADC_CON_ADC_MUX_SEL   21
#define ADC_CON_ADC_MUX_SEL_  3
#define ADC_CON_ADC_ASEL      18
#define ADC_CON_ADC_ASEL_     3
#define ADC_CON_ADC_CLKEN     17
#define ADC_CON_ADC_ISEL      16
#define ADC_CON_WAIT_TIME     12
#define ADC_CON_WAIT_TIME_    4
#define ADC_CON_CH_SEL        8
#define ADC_CON_CH_SEL_       4
#define ADC_CON_PND       7
#define ADC_CON_CPND      6
#define ADC_CON_ADC_IE    5
#define ADC_CON_ADC_EN    4
#define ADC_CON_ADC_AE    3
#define ADC_CON_ADC_BAUD  0
#define ADC_CON_ADC_BAUD_ 3

/* 用户传参时使用带ADC_CH前缀的通道start */
/*                                     */
//AD channel define 普通IO通道
#define ADC_CH_PA0      (ADC_MUX_IO | 0x0)
#define ADC_CH_PA1      (ADC_MUX_IO | 0x1)
#define ADC_CH_PA2      (ADC_MUX_IO | 0x2)
#define ADC_CH_PA3      (ADC_MUX_IO | 0x3)
#define ADC_CH_DP       (ADC_MUX_IO | 0x4)
#define ADC_CH_DM       (ADC_MUX_IO | 0x5)
#define ADC_CH_PA4      (ADC_MUX_IO | 0x6)
#define ADC_CH_PA5      (ADC_MUX_IO | 0x7)
#define ADC_CH_PA6      (ADC_MUX_IO | 0x8)
#define ADC_CH_PA10     (ADC_MUX_IO | 0x9)
#define ADC_CH_PA11     (ADC_MUX_IO | 0xa)
#define ADC_CH_PA12     (ADC_MUX_IO | 0xb)
#define ADC_CH_PA13     (ADC_MUX_IO | 0xc)
#define ADC_CH_PA14     (ADC_MUX_IO | 0xd)
#define ADC_CH_PA15     (ADC_MUX_IO | 0xe)
#define ADC_CH_PB0      (ADC_MUX_IO | 0xf)

//PMU通道
#define ADC_CH_PMU_VBG          (ADC_MUX_AN | 0x0)
#define ADC_CH_PMU_VP17         (ADC_MUX_AN | 0x1)
#define ADC_CH_PMU_VREF_LDO     (ADC_MUX_AN | 0x2)
#define ADC_CH_PMU_MVBG         (ADC_MUX_AN | 0x3)
#define ADC_CH_PMU_VTMP         (ADC_MUX_AN | 0x4)
#define ADC_CH_PMU_1_4_VPWR     (ADC_MUX_AN | 0x5)
#define ADC_CH_PMU_1_4_IOVDD    (ADC_MUX_AN | 0x6)
#define ADC_CH_PMU_1_2_IOVDD    (ADC_MUX_AN | 0x7)
#define ADC_CH_PMU_WVBG         (ADC_MUX_AN | 0x8)
#define ADC_CH_PMU_DVDD_POR_TEST    (ADC_MUX_AN | 0x9)
#define ADC_CH_PMU_DCVDD        (ADC_MUX_AN | 0xa)
#define ADC_CH_PMU_DVDD         (ADC_MUX_AN | 0xb)
#define ADC_CH_PMU_WVDD         (ADC_MUX_AN | 0xd)
//SYSPLL_LDO通道
#define ADC_CH_SYSPLL_LDO       (ADC_MUX_AN| 0xf)//0xf未被占用

//audio通道
#define ADC_CH_AUDIO_IREFN2P5U  (ADC_AUDIO_AN | ADC_AUDIO_SUB | 0x1)
#define ADC_CH_AUDIO_VCM        (ADC_AUDIO_AN | ADC_AUDIO_SUB | 0x2)
#define ADC_CH_AUDIO_DACLDO     (ADC_AUDIO_AN | ADC_AUDIO_SUB | 0x3)
#define ADC_CH_AUDIO_FIFOLDO    (ADC_AUDIO_AN | ADC_AUDIO_SUB | 0x4)
#define ADC_CH_AUDIO_LDAC       (ADC_AUDIO_AN | ADC_AUDIO_SUB | 0x5)
#define ADC_CH_AUDIO_MICBIAS    (ADC_AUDIO_AN | 0x0)
#define ADC_CH_AUDIO_MICLDO     (ADC_AUDIO_AN | 0x1)
#define ADC_CH_AUDIO_ADCVDD     (ADC_AUDIO_AN | 0x2)
#define ADC_CH_AUDIO_QTLDO      (ADC_AUDIO_AN | 0x3)
#define ADC_CH_AUDIO_QTREF      (ADC_AUDIO_AN | 0x4)
#define ADC_CH_AUDIO_BUF_OUT    (ADC_AUDIO_AN | 0x5)

//CLASSD通道
#define ADC_CH_CLASSD_APA       (ADC_CLASSD_AN | 0x0)
/*                                     */
/* 用户传参时使用带ADC_CH前缀的通道end */

#define ADC_MAX_CH_NUM       (10)
#define ADC_VALUE_NONE		 0xffff
#define ADC_CH_NONE		 	 0xff


#define SADC_BIT_WIDE        10
#define SADC_ALL_BITS        ((1UL << SADC_BIT_WIDE) - 1)
#define sadc_value_wrong(n)  ((~SADC_ALL_BITS) & (n))
#define sadc_value_cheak(n)  (0 != sadc_value_wrong(n))

u32 adc_add_sample_ch(u16 real_ch);
void adc_remove_sample_ch(u16 real_ch);
u8 adc_ch2port(u16 real_ch);
void adc_scan(void);
u16 adc_get_value(u16 real_ch);
u16 adc_get_pmu_value(u16 real_ch);
u16 adc_get_io_value(u16 real_ch);
void adc_init(void);
u32 adc_value2voltage(u32 adc_vbg, u32 adc_ch_val);
u32 adc_get_voltage(u16 real_ch);
u32 adc_get_vbat_voltage(void);
u16 adc_get_first_available_ch();
u16 adc_sample_vbg(u32 re_sap_times);
int adc_kick_start(void (*adc_scan_over)(void));
u8 adc_add_ch_reuse(u16 real_ch, u8 busy);
u8 adc_remove_ch_reuse(u16 real_ch);
u32 adc_get_voltage_blocking(u16 real_ch);
u32 adc_sample_value(u16 real_ch, u32 re_sap_times);

#define ADC_CH_PMU_VBAT			ADC_CH_PMU_1_4_VPWR
#define ADC_CH_PMU_1_4VBAT      ADC_CH_PMU_1_4_VPWR
#endif



