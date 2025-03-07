#ifndef __AUDIO_ANALOG_H__
#define __AUDIO_ANALOG_H__

#include "config.h"
#define BIT_APA     BIT(0)
#define BIT_DAA     BIT(1)
#define BIT_CAPLESS BIT(2)
extern u32 audio_analog;
extern u32 app_power_get_vbat(void);

typedef enum _GNU_PACKED_  __ADC_MODE {
    ADC_LINE_IN = 0,
    ADC_MIC,
    ADC_MIC_APA,
    DIGITAL_MIC,
} ADC_MODE;


extern u32 audio_analog;

void audio_voltage_trim();
void audio_analog_open(void);
void audio_analog_close(void);
void audio_analog_lookup(u32 flag, bool mode);
void apa_analog_open(void);
void apa_analog_close(void);
void rdac_analog_open(void);
void rdac_analog_close(void);
void set_apap_output_status(char val);
void set_apan_output_status(char val);
void single_apa_startup_mute_cb(void);
void apa_hardware_mute(u8 mute);

u32 adc_analog_open(ADC_MODE mode, u32 ch);
void adc_analog_close(void);
void dacvdd_ldo(bool mode);

#endif
