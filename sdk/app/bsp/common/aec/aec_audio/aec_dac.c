#include "cpu.h"
#include "config.h"
#include "audio.h"
#include "audio_adc.h"
#include "aec_audio.h"
#include "errno-base.h"
#include "app_config.h"
#include "dac_api.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[aec_audio]"
#include "log.h"

#if 1
short HSuo_ref_cbuf[AEC_REF_POINT * 5 / 3] AT(HSuo_aec_ram); // 存储1.67帧
AEC_AUDIO_STREAM HSuo_ref_astream;

void *regist_aec_ref_2_audio_dac(void)
{
    void *p_cbuf = init_aec_audio_struct(&HSuo_ref_astream, &HSuo_ref_cbuf[0], sizeof(HSuo_ref_cbuf));

    /* if (NULL != p_cbuf) { */
    /* regist_audio_dac_channel((void *)&HSuo_ref_astream.sound, HSuo_kick_aec_run); */
    /* } */
    return p_cbuf;
}
#endif

