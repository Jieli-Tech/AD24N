#ifndef __AUDIO_POWER_H__
#define __AUDIO_POWER_H__

#include "typedef.h"

// audio电源测试一级通路
typedef enum {
    AUD_CH_VCM = 2,
    AUD_CH_LDO,
    AUD_CH_FIFOLDO,
    AUD_CH_LDAC,
    AUD_CH_OTHER,   //即目标通路为二级通路，需要配置AUD_CH_OTHER_SEL
} __AUD_CH;

// audio电源测试二级通路
typedef enum {
    AUD_CH_OTHER_MICBIAS = 0,
    AUD_CH_OTHER_MICLDO,
    AUD_CH_OTHER_ADCVDD,
    AUD_CH_OTHER_QTLDO,
    AUD_CH_OTHER_QTREF,
    AUD_CH_OTHER_BUFOUT,
} __AUD_CH_OTHER;

void audio2saradc_ch_open(__AUD_CH aud2adc_ch, __AUD_CH_OTHER aud2adc_ch_other);
void audio2saradc_ch_close();

#endif

