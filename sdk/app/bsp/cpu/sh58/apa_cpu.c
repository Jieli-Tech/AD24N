/***********************************Jieli tech************************************************
  File : dac_api.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2019-1-14
********************************************************************************************/
/* #include "rdac.h" */
#include "dac_api.h"
#include "dac.h"
#include "config.h"
#include "audio.h"
#include "audio_analog.h"
#include "audio_dac_cpu.h"
#include "audio_apa_cpu.h"
/* #include "asm/power_interface.h" */

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[apa cpu]"
#include "log.h"

const u8 c_apa_iir_par = 1; //IIR滤波器档位,[0,15]; IIR滤波功能默认不开启
const u16 c_apa_dc_dit = 0; //APA DSM直流量,默认为0

APA_PHY_PARA g_apa_para;

void apa_init(u32 sr, bool delay_flag)
{
    HWI_Install(IRQ_APA_SOFT, (u32)apa_isr, IRQ_AUAPA_IP);
    apa_clk_open();

    memset((void *)&g_apa_para, 0, sizeof(g_apa_para));
    if (APA_PWM_192M_EN) {
        bool res = get_apa_para_0(&g_apa_para, sr, APA_DSM_CLK_MODE);
        if (false == res) {
            log_error("apa para error");
            return;
        }

    }
    apa_phy_init(&g_apa_para, APA_CON0_DEFAULT, APA_PWM_MODE);
    audio_apa_analog_init();


    SFR(JL_APA->APA_CON0,  0,  2,  0b11);     // pwm_en dsm_en
    delay(100);
    SFR(JL_APA->APA_CON0, 20,  1,  1);        // apa async_fifo
    delay(100);
    SFR(JL_APA->APA_CON0, 2, 1, 0);  // 解APA数字部分mute
}

/*----------------------------------------------------------------------------*/
/**@brief   关闭APA的函数
   @param   无
   @return  无
   @author  liujie
   @note    void aps_close(void)
*/
/*----------------------------------------------------------------------------*/
void apa_close(void)
{
    SFR(JL_APA->APA_CON0, 20,  1,  0);        // apa async_fifo
    delay(100);
    SFR(JL_APA->APA_CON0,  0,  2,  0);     // pwm_en dsm_en
    audio_apa_analog_close();
}
bool is_apa_close_clear(void)
{
    bool digital = !(!(JL_APA->APA_CON0 & ((1 << 20) | (3 << 0))));
    bool analog = audio_apa_analog_is_close_clear();
    return !(digital || analog);
}
void apa_n_highz(void)
{
    SFR(JL_APA->APA_CON3, 13, 1, 0);     //CLASSD_ION_MODE_11v
    SFR(JL_APA->APA_CON3, 15, 1, 0);     //CLASSD_N_EN_11v
}
void apa_p_highz(void)
{
    SFR(JL_APA->APA_CON3, 14, 1, 0);     //CLASSD_IOP_MODE_11v
    SFR(JL_APA->APA_CON3, 16, 1, 0);     //CLASSD_P_EN_11v
}

void dump_aps_sfr(void)
{
    log_info("JL_APA->APA_CON0    = %8xH", JL_APA->APA_CON0);
    /* log_info("JL_APA->APA_CON1    = %8xH", JL_APA->APA_CON1); */
    /* log_info("JL_APA->APA_CON2    = %8xH", JL_APA->APA_CON2); */
    /* log_info("JL_APA->APA_CON3    = %8xH", JL_APA->APA_CON3); */
    /* log_info("JL_APA->APA_CON4    = %8xH", JL_APA->APA_CON4); */

}

