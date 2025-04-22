#include "typedef.h"

/* ***********************
 * 在不同供电场景下config_adda_voltage_mode配置不同的值：
 * 0：高压供电vbat，工作电压在2.7v ~ 5.4v之间
 * 1：低压供电到iovdd&vbat，工作电压在1.8v ~ 3.6v之间
 * 注：低压供电时，iovdd需要和vabt短接；
 * 注：此项配置只会影响到audio dac & audio adc的性能；不会对APA性能产生影响
 * ************************/
const unsigned char config_adda_low_voltage_mode = 0;


//*********************************************************************************//
//                        AUDIO  Configuration                                     //
//*********************************************************************************//
/*
+============================+==============+==============+==============+
|         AD24N AUDIO & APA 工作依赖常量                                  |
+----------------------------+--------------+--------------+--------------+
|                            | Audio DAC    | Audio ADC    | Audio APA    |
+----------------------------+--------------+--------------+--------------+
|   au_const_apa_en          | ✘            | ✘            | ✔            |
+----------------------------+--------------+--------------+--------------+
|   au_const_dac_digital_en  | ✔            | ✘            | ✔            |
+----------------------------+--------------+--------------+--------------+
|   au_const_dac_analog_en   | ✔            | ✘            | ✘            |
+----------------------------+--------------+--------------+--------------+
|   au_const_adda_common_en  | ✔            | ✔            | ✘            |
-----------------------------+--------------+--------------+--------------+
*/
const u8 au_const_apa_en = 1;
const u8 au_const_dac_digital_en = 1;
const u8 au_const_dac_analog_en = 1;
const u8 au_const_adda_common_en = 1;


//*********************************************************************************//
//                        AUDIO ADC  Configuration                                 //
//*********************************************************************************//

const char MIC_PGA_G        = 14;    //0 ~ 14,无效参数

#include "audio_adc_cpu.h"
const bool audio_adc_diff_mic_mode = 0;// 差分amic使能(N端固定PA15) 0:单端amic  1:差分amic
const bool audio_adc_diff_aux_mode = 0;// 差分aux使能(N端固定PA15) 0:单端aux  1:差分aux

const AUADC_PERFORM_MODE    auadc_perf_mode = AUADC_STANDARD_PERF; //audio_adc性能
const AUDIO_MIC_RS_MODE     auadc_mic_rs_pwr_mode = MIC_RS_INSIDE;
const AUDIO_MICPGA_G        audio_adc_mic_pga_g = AUMIC_27db;//后级增益
const AUDIO_MICBIAS_RS      audio_adc_mic_bias_rs = AUMIC_1k5;//micbias内部偏置电阻选择
const AUDIO_MICLDO_VS       audio_adc_mic_ldo_vs = AUMIC_2v0;//micldo偏置电压选择


const AUDIO_MIC_INPUT_PORT  audio_adc_mic_input_port = MIC_INPUT_ANA1_PB1;//micin输入口
const AUDIO_MIC_INPUT_PORT  audio_adc_aux_input_port = MIC_INPUT_ANA2_PA14;//linein输入口
const u8 audio_adc_dcc = 1; //0~14:滤波器系数，档位越低，截止点越低 15:bypass


