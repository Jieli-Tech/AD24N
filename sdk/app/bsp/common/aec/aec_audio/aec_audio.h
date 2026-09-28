#ifndef _AEC_AUDIO_H_
#define _AEC_AUDIO_H_

#include "typedef.h"

#include "circular_buf.h"
#include "sound_effect_api.h"


#define TCFG_AEC_DAC_SR         SR_DEFAULT
#define TCFG_AEC_ADC_SR         16000

#define READSIZE 	            256      /* 每帧256点，符合AI_ECNR要求 */
#define AEC_SR_COEF             (TCFG_AEC_DAC_SR / TCFG_AEC_ADC_SR)
#define AEC_NEAR_POINT          (READSIZE)
#define AEC_REF_POINT           (AEC_NEAR_POINT * AEC_SR_COEF)



typedef struct {
    cbuffer_t cbuf;
    sound_out_obj sound;
} AEC_AUDIO_STREAM;

cbuffer_t *init_aec_audio_struct(AEC_AUDIO_STREAM *ps, void *obuf, u32 obuf_size);
void *regist_aec_ref_2_audio_dac(void);
void *regist_aec_mic_2_audio_dac(void);
void enable_HSsuo_ref_sound(void);

extern bool HSuo_kick_aec_run(void *p_sound_out);
#endif

