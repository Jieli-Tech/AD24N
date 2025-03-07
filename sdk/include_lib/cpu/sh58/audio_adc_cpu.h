#ifndef __AUDIO_ADC_CPU_H__
#define __AUDIO_ADC_CPU_H__

#include "typedef.h"

//=============================================================================
//=                                                                           =
//=                       Audio adc Physics Definition                        =
//=                                                                           =
//=============================================================================

//------------------------------------------
// AUDIO ADC 是否工作在低电耗模式
// * 低电耗模式，耗电下降，性能也会下降
//--------------------------------------------
typedef enum __AUADC_PERFORM_MODE {
    AUADC_STANDARD_PERF = 0,  //标准性能配置
    AUADC_LOWPOWER_PERF = 1, //低功耗性能配置
} AUADC_PERFORM_MODE; //外部供电一定时，AUDIO_ADC的性能配置

/**
 * AUDIO ADC MIC的三种工作模式选择 涉及
 * MIC内置偏置电阻的使能与选择
 * MIC LDO电源使能与输送到PA13
 * MIC LDO电源反馈点选择pad还是内部；
 */
typedef enum __AUMIC_RS_MODE {
    MIC_RS_INSIDE = 0,  //PA13输出MICLDO供电, 内部电源+内部偏置电阻
    MIC_RS_OUTSIDE = 1, //PA13输出MICLDO供电, 内置电源+外部偏置电阻
    MIC_RS_NULL = 3,    //外置电源+外置电阻
} AUDIO_MIC_RS_MODE;




typedef enum __AUMIC_INPUT_PORT {
    MIC_INPUT_ANA0_PA13 = 0,          //PA13省电容mic输入(PA13同时固定为micldo/midbias输出,不可用作差分)
    MIC_INPUT_ANA1_PB1  = 1,          //PB1 单端MIC输入
    MIC_INPUT_ANA2_PA14 = 2,          //PA14单端MIC输入
    MIC_INPUT_ANA3_APAP = 3,          //APAP&APAN差分输入,使用时需要将差分配置audio_adc_diff_mic_mode打开(使用该通路时，只能APAP&APAN差分输入，且输入时不能使用APA输出)
} AUDIO_MIC_INPUT_PORT;

typedef enum __AUDIO_MICLDO_VS {
    AUMIC_2v0 = 0, //高压对应2.0v,低压对应1.6v
    AUMIC_2v2 = 1, //高压对应2.1v,低压对应1.7v
    AUMIC_2v4 = 2, //高压对应2.2v,低压对应1.8v
    AUMIC_2v6 = 3, //高压对应2.3v,低压对应1.9v
    AUMIC_2v7 = 4, //高压对应2.4v,低压对应2.0v
    AUMIC_2v8 = 5, //高压对应2.6v,低压对应2.2v
    AUMIC_2v9 = 6, //高压对应2.8v,低压对应2.4v
    AUMIC_3v0 = 7, //高压对应3.0v,低压对应2.6v
} AUDIO_MICLDO_VS;
extern AUDIO_MICLDO_VS const audio_adc_mic_ldo_vs;

typedef enum __AUDIO_MICBIAS_RS {
    AUMIC_0k5 = 0,
    AUMIC_1k0 = 1,
    AUMIC_1k5 = 2,
    AUMIC_2k0 = 3,
    AUMIC_2k5 = 4,
    AUMIC_3k0 = 5,
    AUMIC_3k5 = 6,
    AUMIC_4k0 = 7,
    AUMIC_4k5 = 8,
    AUMIC_5k0 = 9,
    AUMIC_6k0 = 0xa,
    AUMIC_7k0 = 0xb,
    AUMIC_8k0 = 0xc,
    AUMIC_9k0 = 0xd,
    AUMIC_10k = 0xe,
    AUMIC_RS_READ = 0xf,
} AUDIO_MICBIAS_RS;

typedef enum __AUDIO_MICPGA_G {
    // MIC_PGA增益档位说明
    AUMIC_0db = 0,
    AUMIC_3db = 1,
    AUMIC_9db = 2,
    AUMIC_15db = 3,
    AUMIC_21db = 4,
    AUMIC_27db = 5,
} AUDIO_MICPGA_G;

#define AUADC0_MIC_INSIDE_RS_EN         (1 << 13)
#define AUADC0_MICA_BUF_IS(n)       ((n & 0x3) << 19)
#define AUADC0_ADCLDO_IS(n)         ((n & 0x3) << 7)

#define AUADC0_BITS                 ((3 << 19) | ( 0x1f<<13) | (3<<7))

#define AUADC2_MICLDO_EN            (1 << 0)
#define AUADC2_MICLDO_IS(n)         ((n & 0x3) << 1)
#define AUADC2_MICLDO_PA13_EN       (1 << 3)
#define AUADC2_QTLDO_IS(n)          ((n & 0x3) << 9)
#define AUADC2_QTREF_IS(n)          ((n & 0x3) << 14)
#define AUADC2_SDM_ISH(n)           ((n & 0x3) << 23)
#define AUADC2_SDM_ISL(n)           ((n & 0x3) << 25)

#define AUADC2_BITS           \
        ( (0x7<<0) | (1<<3) | (3<<9) | (3<<14) |(0xf<<23))

#define AUADC4_MICLDO_DRV_SEL_PAD        (1 << 5)
#define AUADC4_BITS                      (1 << 5)


// #define AUDIO_ADC_RS_NORMAL_MODE    0 //mic电路使用外置电阻，外置电源
// #define AUDIO_ADC_RS_INSIDE_MODE    1 //mic电路使用内置电阻，内部电源
// #define AUDIO_ADC_RS_OUTSIDE_MODE   0 //mic电路使用外置电阻，内部电源

#define AUADC_RS_IN_CON0  AUADC0_MIC_INSIDE_RS_EN
#define AUADC_RS_IN_CON2  AUADC2_MICLDO_EN
#define AUADC_RS_IN_CON4  0

#define AUADC_RS_OUT_CON0 0
#define AUADC_RS_OUT_CON2  AUADC2_MICLDO_EN | AUADC2_MICLDO_PA13_EN
#define AUADC_RS_OUT_CON4  AUADC4_MICLDO_DRV_SEL_PAD


// #define AUADC_LOWPOWER_PERF_MODE     0 //AUDIO ADC低电耗使能

#define AUADC_POWER_LOW_CON0   (AUADC0_ADCLDO_IS(1) | \
                           AUADC0_MICA_BUF_IS(1) )
#define AUADC_POWER_LOW_CON2   (AUADC2_MICLDO_IS(1) | \
                            AUADC2_QTLDO_IS(1)  | \
                            AUADC2_QTREF_IS(1)  | \
                            AUADC2_SDM_ISH(1)   | \
                            AUADC2_SDM_ISL(1) )

#define AUADC_POWER_NORMAL_CON0   (AUADC0_ADCLDO_IS(2) | \
                           AUADC0_MICA_BUF_IS(2) )
#define AUADC_POWER_NORMAL_CON2   (AUADC2_MICLDO_IS(2) | \
                            AUADC2_QTLDO_IS(2)  | \
                            AUADC2_QTREF_IS(2)  | \
                            AUADC2_SDM_ISH(2)   | \
                            AUADC2_SDM_ISL(2) )
//-------------------------------------------------------------
typedef struct __AUADC_SFR {
    u32 ada_con0;
    u32 ada_con2;
    // u32 ada_con3;
    u32 ada_con4;
} AUADC_SFR;
typedef struct __AUADC_POWER_SFR {
    u32 ada_con0;
    u32 ada_con2;
} AUADC_POWER_SFR;
//-------------------------------------------------------------


#define APA_CON0_DEFAULT   (APA_PWM_MODE)






// // 计算启用的宏的数量
// #define AUADC_MODE_COUNT (AUDIO_ADC_RS_NORMAL_MODE + AUDIO_ADC_RS_INSIDE_MODE + AUDIO_ADC_RS_OUTSIDE_MODE)

// // 检查是否只有一个宏被启用
// #if AUADC_MODE_COUNT == 0
// #error "必须至少启用一个功能宏 (AUDIO_ADC_RS_NORMAL_MODE, AUDIO_ADC_RS_INSIDE_MODE, AUDIO_ADC_RS_OUTSIDE_MODE)"
// #elif AUADC_MODE_COUNT > 1
// #error "只能启用一个功能宏 (AUDIO_ADC_RS_NORMAL_MODE, AUDIO_ADC_RS_INSIDE_MODE, AUDIO_ADC_RS_OUTSIDE_MODE)"
// #endif

void audio_adc_analog_init(const AUADC_SFR *c_sfr, const AUADC_POWER_SFR *c_power_sfr, u8 mic_pga_gain);


typedef struct __AUDIO_ADC_CTRL_HDL {
    void *buf;
    u16 pns;
    u16 sp_total;
    u8  sp_size;
    u16 throw;//抛弃开始的数据包数目
} AUDIO_ADC_CTRL_HDL;

#define AUDIO_ADC_SP_BITS     16
#define AUDIO_ADC_PACKET_SIZE (48*5)
#define AUDIO_ADC_SP_SIZE     (AUDIO_ADC_SP_BITS / 8)
#define AUDIO_ADC_SP_PNS      (AUDIO_ADC_PACKET_SIZE / 3)

void auadc_clk_open(void);
void au_dmic_clk_open(void);
void au_dmic_clk_close(void);
void fadc_init(const AUDIO_ADC_CTRL_HDL *ops);
void audio_adc_analog_close();
void audio_mic_sel();
void audio_adc_enable_phy();

typedef struct __AUDIO_ADC_DIG_MIC_HDL {
    u8 clk_io;
    u8 data_io;
    u8 data_ch;
    bool enable;
} AUDIO_ADC_DIG_MIC_HDL;

extern const AUDIO_MICBIAS_RS      audio_adc_mic_bias_rs;//micbias内部偏置电阻选择
extern const AUDIO_MICLDO_VS       audio_adc_mic_ldo_vs;//micldo偏置电压选择
extern const u8 audio_adc_dcc;

#endif
