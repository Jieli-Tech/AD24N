#include "cpu.h"
#include "config.h"
#include "circular_buf.h"
#include "audio.h"
#include "audio_adc.h"
#include "aec_audio.h"
#include "errno-base.h"
#include "app_config.h"
#include "dac_api.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[aec_audio]"
#include "log.h"



short HSuo_near_cbuf[AEC_NEAR_POINT * 5 / 3] AT(HSuo_aec_ram); // 存储1.67帧
AEC_AUDIO_STREAM HSuo_mic_astream;


cbuffer_t *init_aec_audio_struct(AEC_AUDIO_STREAM *ps, void *obuf, u32 obuf_size)
{
    if (NULL == ps) {
        return NULL;
    }
    memset(ps, 0, sizeof(AEC_AUDIO_STREAM));
    cbuf_init(&ps->cbuf, obuf, obuf_size);
    ps->sound.p_obuf = &ps->cbuf;
    return ps->sound.p_obuf;

}

void *regist_aec_mic_2_audio_dac(void)
{
    log_info("regist_aec_mic_2_audio_dac");
    void *p_cbuf = init_aec_audio_struct(&HSuo_mic_astream, &HSuo_near_cbuf[0], sizeof(HSuo_near_cbuf));

    if (NULL != p_cbuf) {
        log_info("regist_aec_mic_2_audio_dac succ");
        regist_audio_adc_channel((void *)&HSuo_mic_astream.sound, HSuo_kick_aec_run);
    }
    return p_cbuf;
}

void enable_HSsuo_ref_sound(void)
{
    HSuo_mic_astream.sound.enable |= B_DEC_RUN_EN | B_DEC_FIRST;
}
