/***********************************Jieli tech************************************************
  File : audio_dac_apa_switch.c
  Email:
  date : 2026-07-24
  Description: Independent per-channel control for audio DAC and APA output paths.

********************************************************************************************/
#include "audio_dac_apa_switch.h"
#include "audio_dac_cpu.h"
#include "audio_apa_cpu.h"
#include "dac.h"
#include "dac_api.h"
#include "sfr.h"
#include "cpu.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[AU_SW]"
#include "log.h"

/* Actual hardware state — source of truth for whether init/close is needed */
static bool g_dac_analog_on = true;
static bool g_apa_on        = true;

/*----------------------------------------------------------------------------*/
/**@brief   Initialize the switch module.
            Detects hardware state after audio_dac_init() and applies
            the boot mode from cpu_config.c.
*/
/*----------------------------------------------------------------------------*/
void audio_switch_init()
{
    /* Detect actual hardware state */
    if ((au_const_apa_en) && (au_const_dac_digital_en)) {
        g_apa_on = 1;
    } else {
        g_apa_on = 0;
    }
    if ((au_const_dac_digital_en) && (au_const_dac_analog_en) && (au_const_adda_common_en)) {
        g_dac_analog_on = 1;
    } else {
        g_dac_analog_on = 0;
    }

}

/*----------------------------------------------------------------------------*/
/**@brief   Enable or disable the DAC output independently.
   @param   on  : true=enable DAC analog, false=disable
   @note    Only toggles DAC analog (DAA_CON0/1/2). DAC digital pipeline
            is shared with APA and managed automatically.
*/
/*----------------------------------------------------------------------------*/
void audio_switch_set_dac(bool on)
{

    if (on == g_dac_analog_on) {
        return;     /* already in desired state */
    }

    u32 sr = dac_sr_read();
    if (0 == sr) {
        log_error("dac no open\n");
        return;
    }
    dac_mute(true);

    if (on) {
        if (0 != au_const_dac_analog_en) {
            adda_dac_analog_init();
        }
        g_dac_analog_on = true;
        log_info("now dac open\n");
    } else {
        adda_dac_analog_close();
        g_dac_analog_on = false;
        log_info("now dac close\n");
    }

    dac_mute(false);
}

/*----------------------------------------------------------------------------*/
/**@brief   Enable or disable the APA output independently.
   @param   on  : true=enable APA, false=disable
*/
/*----------------------------------------------------------------------------*/
void audio_switch_set_apa(bool on)
{

    if (on == g_apa_on) {
        return;     /* already in desired state */
    }

    dac_mute(true);
    u32 sr = dac_sr_read();
    if (0 == sr) {
        log_error("dac no open\n");
        return;
    }
    if (on) {
        apa_init(sr, 1);
        g_apa_on = true;
        log_info("now apa close\n");
    } else {
        apa_close();
        apa_n_highz();
        apa_p_highz();
        apa_clk_close();
        g_apa_on = false;
        log_info("now apa close\n");
    }

    dac_mute(false);
}

/*----------------------------------------------------------------------------*/
/**@brief   Query whether DAC analog is currently active.
   @return  true if DAC analog is on
*/
/*----------------------------------------------------------------------------*/
bool audio_switch_get_dac(void)
{
    return g_dac_analog_on;
}

/*----------------------------------------------------------------------------*/
/**@brief   Query whether APA is currently active.
   @return  true if APA is on
*/
/*----------------------------------------------------------------------------*/
bool audio_switch_get_apa(void)
{
    return g_apa_on;
}

