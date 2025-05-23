/***********************************Jieli tech************************************************
  File : speak_api.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2021年6月15日
********************************************************************************************/
#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "audio.h"
#include "dac.h"
#include "audio_adc.h"
#include "circular_buf.h"
#include "dac_api.h"
#include "sound_effect_api.h"
#include "errno-base.h"
#include "sine_play.h"
#include "src_api.h"
#include "app_modules.h"
#if defined(VO_PITCH_EN) && (VO_PITCH_EN)
#include "vo_pitch_api.h"
#endif
#if defined(VO_CHANGER_EN) && (VO_CHANGER_EN)
#include "voiceChanger_av_api.h"
#endif
#if defined(PCM_EQ_EN) && (PCM_EQ_EN)
#include "pcm_eq_api.h"
#endif
#if defined(ECHO_EN) && (ECHO_EN)
#include "echo_api.h"
#endif
#if defined(NOTCH_HOWLING_EN) && (NOTCH_HOWLING_EN)
#include "notch_howling_api.h"
#endif
#if defined(PITCHSHIFT_HOWLING_EN) && (PITCHSHIFT_HOWLING_EN)
#include "howling_pitchshifter_api.h"
#endif

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[speak_api]"
#include "log.h"

extern const char MIC_PGA_G;

cbuffer_t cbuf_aas_o AT(.speaker_data);
u8 obuf_aas_o[1024]  AT(.speaker_data);

cbuffer_t cbuf_ads_o AT(.speaker_data);
u8 obuf_ads_o[1024]  AT(.speaker_data);
/* sound_out_obj audio_dac_sound; */

typedef struct __audio_adc_speaker {
    sound_out_obj  sound;
    sound_out_obj  *p_dac_sound;
    EFFECT_OBJ    *p_src;            // p_curr_sound->effect;
    EFFECT_OBJ    *p_vp;            // p_curr_sound->effect;
} __aa_speaker;

__aa_speaker aa_speaker AT(.speaker_data);


void audio_adc_speaker_start(void)
{
    /* if (aa_speaker.sound.enable & B_DEC_RUN_EN) { */
    /* log_info("SPEAKER START ERR !!! \n"); */
    /* return; */
    /* } */

    log_info("SPEAKER START !!! \n");

    memset(&aa_speaker, 0, sizeof(aa_speaker));
    stream_sound_init(&aa_speaker.sound, NULL);

    cbuf_init(&cbuf_aas_o, &obuf_aas_o[0], sizeof(obuf_aas_o));
    aa_speaker.sound.p_obuf = &cbuf_aas_o;
    audio_adc_init_api(24000, ADC_MIC, audio_adc_mic_input_port);

    u32 adc_sr = read_audio_adc_sr();
    u32 dac_sr = dac_sr_read();


    sound_out_obj *p_curr_sound = 0;
    p_curr_sound = &aa_speaker.sound;

    cbuf_init(&cbuf_ads_o, &obuf_ads_o[0], sizeof(obuf_ads_o));


#if defined(HOWLING_EN) && (HOWLING_EN) //陷波抑制啸叫
    p_curr_sound = link_notch_howling_sound(p_curr_sound, &cbuf_ads_o, 0, adc_sr);
#endif
#if defined(HOWLING_EN) && (HOWLING_EN) //移频抑制啸叫
    p_curr_sound = link_pitchshift_howling_sound(p_curr_sound, &cbuf_ads_o, 0, adc_sr);
#endif
    /*voice_toy工程 混响、voice_pitch、voice_changer、eq资源复用，只能开其中一个，或者自行调整ld文件 */
#if defined(ECHO_EN) && (ECHO_EN) //混响
    p_curr_sound = link_echo_sound(p_curr_sound, &cbuf_ads_o, 0, adc_sr);
#endif
#if defined(VO_PITCH_EN) && (VO_PITCH_EN) //多种音效集合库：机器人、rap、两种变速变调；
    p_curr_sound = link_voice_pitch_sound(p_curr_sound, &cbuf_ads_o, (void **)&aa_speaker.p_vp, VP_CMD_ROBOT);
#endif
#if defined(VO_CHANGER_EN) && (VO_CHANGER_EN)
    p_curr_sound = link_voice_changer_sound(p_curr_sound, &cbuf_ads_o, 0, adc_sr);
#endif
#if defined(PCM_EQ_EN) && PCM_EQ_EN
    p_curr_sound = link_pcm_eq_sound(p_curr_sound, &cbuf_ads_o, 0, adc_sr, 1);
#endif


#if (defined(HAS_HW_SRC_EN) || defined(HAS_SW_SRC_EN))
    if (adc_sr != dac_sr) {
        p_curr_sound = link_src_sound(p_curr_sound, &cbuf_ads_o, (void **)&aa_speaker.p_src, adc_sr, dac_sr, (void *)GET_SRC_OPS());
    };
#endif
    if (&aa_speaker.sound == p_curr_sound) {
        regist_audio_adc_channel(&aa_speaker.sound, (void *) NULL);
    } else {
        regist_audio_adc_channel(&aa_speaker.sound, (void *) kick_sound);

    }
    regist_dac_channel(p_curr_sound, NULL);//注册到DAC;
    aa_speaker.p_dac_sound = p_curr_sound;

    /* aa_speaker.p_sound = &aa_speaker.sound; */


    audio_adc_enable(MIC_PGA_G);
    aa_speaker.sound.enable |= B_DEC_RUN_EN | B_DEC_FIRST;
    p_curr_sound->enable |= B_DEC_RUN_EN | B_DEC_FIRST;

}

void audio_adc_speaker_reless(void)
{
    if (aa_speaker.sound.enable & B_DEC_RUN_EN) {
        log_info("SPEAKER RELESS !!! \n");
        aa_speaker.sound.enable &= ~B_DEC_RUN_EN;
        stream_sound_uninit();
        audio_adc_disable();
        auin_off_api();
        unregist_audio_adc_channel(&aa_speaker.sound);

        unregist_dac_channel(aa_speaker.p_dac_sound);

#if (defined(HAS_HW_SRC_EN) || defined(HAS_SW_SRC_EN))
        if (NULL != aa_speaker.p_src) {
            src_reless((void **)&aa_speaker.p_src);
        }
#endif

        memset(&aa_speaker, 0, sizeof(aa_speaker));
    }

}













