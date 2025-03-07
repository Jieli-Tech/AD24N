#ifndef __DAC_CPU_H__
#define __DAC_CPU_H__


#include "typedef.h"
#include "audio_cpu.h"

//dac 输出采样率
#define AUDAC_SR96K       (0b0000)
#define AUDAC_SR88K2      (0b0001)
#define AUDAC_SR64K       (0b0010)
#define AUDAC_SR48K       (0b0100)
#define AUDAC_SR44K1      (0b0101)
#define AUDAC_SR32K       (0b0110)
#define AUDAC_SR24K       (0b1000)
#define AUDAC_SR22K05     (0b1001)
#define AUDAC_SR16K       (0b1010)
#define AUDAC_SR12K       (0b1100)
#define AUDAC_SR11K025    (0b1101)
#define AUDAC_SR8K        (0b1110)
#define AUDAC_SRBITS      (0b1111)


#define DAC0_DEM_EN          (1 << 31)
#define DAC0_DEM_LS(n)       ((n & 3) << 28)
#define DAC0_DITHER(n)       ((n & 3) << 26)   //0.1.2.3 : div64.32.16.8
#define DAC0_DITHER_SEL(n)   ((n & 1) << 5)    //0:正弦波;  1: 方波
#define DAC0_DITHER_EN       (1 << 24)
#define DAC0_IIR_PAR(n)      ((n & 3) << 22)
#define DAC0_CHEN            (1 << 16)         //   19:16
#define DAC0_CHPHSET         (14 << 12)        //DACCHPSET, 去直流滤波器配置，建议设置为14
#define DAC0_APA_SEL         (1 << 9)
#define DAC0_RELEASE_RST     (1 << 8)
#define DAC0_PND             (1 << 7)
#define DAC0_CPND            (1 << 6)
#define DAC0_IE              (1 << 5)
#define DAC0_DMA_EN          (1 << 4)


#define DAC1_DFIFO_PND        (1 << 30)
#define DAC1_DFIFO_CPND       (1 << 29)
#define DAC1_DFIFO_EN         (1 << 28)
#define DAC1_DFIFO_SYNC       (1 << 21)
#define DAC1_DFIFO_CLKSEL(n)  ((n & 1) << 20)
#define DAC1_FADE_BUSY        (1 << 15)
#define DAC1_FADE_EN          (1 << 14)
#define DAC1_VOL_INV          (1 << 13)
#define DAC1_ACDIT_SEL(n)     ((n & 7) << 10)
#define DAC1_FADE_SLOW(n)     ((n & 0xf) << 4)
#define DAC1_FADE_STEP(n)     ((n & 0xf) << 0)

extern const u8 au_const_apa_en;
extern const u8 au_const_dac_digital_en;
extern const u8 au_const_dac_analog_en;
extern const u8 au_const_adda_common_en;



#define DAC_CON0_DEFAULT  \
    ( \
      DAC0_IE        |\
      DAC0_DEM_EN    |\
      DAC0_DEM_LS(3) |\
      DAC0_CHEN      |\
      DAC0_DMA_EN    |\
      DAC0_CHPHSET   |\
      DAC0_DITHER(3) \
    )

#define DAC_CON1_DEFAULT  \
    ( \
      DAC1_DFIFO_SYNC  \
    )


#define SR_DEFAULT  32000


typedef struct _DAC_CTRL_HDL {
    void *buf;
    u32  con0;
    u32  con1;
    u32  pns;          //dac中断门槛
    u16  sp_total;
    u16  sp_max_free;  //填充数据后允许的最大Free空间，不够填零
    /* u8   sp_size; */
} DAC_CTRL_HDL;

void audio_dac_analog_init();


void adda_dac_analog_close(void);
void adda_dac_analog_init();
void adda_clk_open(u32 sr);
void adda_clk_close(void);
void fdac_resource_init(const DAC_CTRL_HDL *ops);

void apa_clk_sel(void);
void auadc_clk_open(void);

void dac_isr(void);
void rdac_phy_vol(u16 mono_vol);


#define D_PHY_VOL_SET_FUNC  rdac_phy_vol


#define dac_sp_handle(n)  do {                              \
                            static signed char ii = 1;      \
                            if (ii > 0) {                   \
                                ii = -1;                    \
                            } else {                        \
                                ii = 1;                     \
                            }                               \
                            (n) += ii;                      \
                        } while(0)

#endif
