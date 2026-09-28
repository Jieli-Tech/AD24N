#ifndef __AUDIO_DAC_APA_SWITCH_H__
#define __AUDIO_DAC_APA_SWITCH_H__

#include "typedef.h"

/**
 * @brief Boot-time audio output mode configuration.
 *
 * Set this in cpu_config.c to choose the initial output mode after power-on:
 *   AUDIO_OUTPUT_DAC  (1) - DAC only at boot, APA off
 *   AUDIO_OUTPUT_APA  (2) - APA only at boot, DAC off
 *   AUDIO_OUTPUT_BOTH (3) - Both active at boot (default)
 */
typedef enum {
    AUDIO_OUTPUT_DAC  = 1,
    AUDIO_OUTPUT_APA  = 2,
    AUDIO_OUTPUT_BOTH = 3,
} audio_output_mode_t;

/**
 * @brief Initialize the switch module. Must be called once after dac_init_api().
 *        Reads audio_boot_output_mode and applies the boot-time configuration.
 */
void audio_switch_init();

/**
 * @brief Enable or disable the DAC output independently.
 *        Only toggles DAC analog (DAA_CON0/1/2). DAC digital pipeline
 *        is shared with APA and managed automatically.
 * @param on  true = enable DAC, false = disable
 */
void audio_switch_set_dac(bool on);

/**
 * @brief Enable or disable the APA output independently.
 * @param on  true = enable APA, false = disable
 */
void audio_switch_set_apa(bool on);

/**
 * @brief Query whether DAC analog is currently active.
 */
bool audio_switch_get_dac(void);

/**
 * @brief Query whether APA is currently active.
 */
bool audio_switch_get_apa(void);

/**
 * @brief Boot-time output mode. Define in cpu_config.c:
 *        AUDIO_OUTPUT_DAC / AUDIO_OUTPUT_APA / AUDIO_OUTPUT_BOTH
 */
extern const u8 audio_boot_output_mode;

#endif /* __AUDIO_DAC_APA_SWITCH_H__ */

