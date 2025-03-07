#ifndef __AUDIO_CPU_H__
#define __AUDIO_CPU_H__

#include "typedef.h"
#include "asm/power_interface.h"

void audio_clk_open(u32 dly);
void audio_clk_close(void);
void adda_clk_open(u32 sr);
void adda_clk_close(void);

extern const unsigned char config_adda_low_voltage_mode;

#define audio_analog_open   audio_common_analog_open
#define audio_analog_close  audio_common_analog_close
#define audio_isr_init()
#define audio_clk_init()    {  \
                                audio_clk_open(2000); \
                            }
#define audio_clk_close     audio_clk_close
#define audio_get_iovdd_vol()  get_vddiom_vol()

extern const u8 config_adda_low_voltage_vbg;
#endif


