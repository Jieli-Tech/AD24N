#ifndef __APA_H__
#define __APA_H__

#include "typedef.h"

//=============================================================================
//=                                                                           =
//=                       Audio APA Physics Definition                        =
//=                                                                           =
//=============================================================================

//----------------------------------
#define APA0_PND              (1 << 23)
#define APA0_CPND             (1 << 22)
#define APA0_IE_EN            (1 << 21)

#define APA0_FIFO_RST         (1 << 20)
#define APA0_FIFOSYNC_EN      (1 << 19)
#define APA0_IIR_EN           (1 << 16)

#define APA0_PWM_DOR(n)       ((n & 0x3ff) << 6)

#define APA0_MUTE             (1 << 2)
#define APA0_PWM_EN           (1 << 1)
#define APA0_DSM_EN           (1 << 0)

#define APA0_PBTL_MODE        (1 << 5)

#define APA0_SINGLE           (0 << 3)
#define APA0_DIFF_SNR         (1 << 3)
#define APA0_DIFF_THD         (2 << 3)

#define APA1_IIR_PAR(n)       ((n & 0xf) << 20)

/*******************************/
#define APA_PWM_192M_EN     1
#define APA_DSM_CLK_MODE    0       // 0:384kHz   352.9kHz     low clk
// #define APA_PWM_MODE        APA0_DIFF_THD
#define APA_PWM_MODE        APA0_DIFF_SNR   //开发板硬件默认为差分

#define APA_CON0_DEFAULT   (APA_PWM_MODE)

typedef struct _APA_PHY_PARA {
    u16 pwm_dor;
    u8  dsm_dor;
} APA_PHY_PARA;

void audio_apa_analog_init();
void apa_close();

void audio_apa_analog_close(void);
void apa_init(u32 sr, bool delay_flag);
// apa库接口
void apa_clk_open();
void apa_clk_close();
void apa_isr(void);
bool get_apa_para_0(APA_PHY_PARA *p_apa, u32 sr, u32 dsm_clk_mod);
bool apa_phy_init(APA_PHY_PARA *p_apa, u32 con0, u32 pwm_mode);
void apa_phy_off(void);
bool is_apa_close_clear(void);
void apa_n_highz(void);
void apa_p_highz(void);
bool audio_apa_analog_is_close_clear(void);
#endif
