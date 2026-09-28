#include "aec_app.h"
#include "app_config.h"
#include "audio.h"
#include "audio_adc.h"
#include "dac_api.h"
#include "sound_effect_api.h"
#include "audio_analog.h"
/* #include "simple_decode.h" */
#include "msg.h"
#include "key.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[aec_app]"
#include "log.h"

extern const char MIC_PGA_G;   // 麦克风增益（虚设参数）

/**
 * @brief 初始化麦克风及 AEC
 */
void aec_app_init(void)
{
    /* // 1. 初始化 ADC 硬件 */
    /* audio_adc_init_api(TCFG_MIC_ADC_SAMP, ADC_MIC, audio_adc_mic_input_port); */
    audio_adc_init_api(16000, ADC_MIC, audio_adc_mic_input_port);
    audio_adc_enable(MIC_PGA_G);

    /* // 2. 省电容校准（若硬件支持） */
    /* #if defined(TCFG_MIC_CAPLESS) && (TCFG_MIC_CAPLESS == 1) */
    /* audio_adc_trim(); */
    /* #endif */

    extern u32 nlp_api(u32 sr);
    nlp_api(16000);

    log_info("aec_app_init done\n");
}


void aec_app_process(void)
{

}


