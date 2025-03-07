
#include "audio_signal.h"

/*----------------------------------------------------------------------------*/
/**@brief   audio 相关电源测试通路选择函数
   @param   aud2adc_ch:一级通路选择
   @param   aud2adc_ch_other:二级通路选择,该通路需要一级通路选择AUD_CH_OTHER才有效
   @return  无
   @author  liuhaokun
   @note    void audio2saradc_ch_open(__AUD_CH aud2adc_ch, __AUD_CH_OTHER aud2adc_ch_other)
*/
/*----------------------------------------------------------------------------*/
void audio2saradc_ch_open(__AUD_CH aud2adc_ch, __AUD_CH_OTHER aud2adc_ch_other)
{
    u32 ch = 0;
    ch = BIT(aud2adc_ch) | ((aud2adc_ch_other & 0x7) << 7);
    SFR(JL_ADDA->ADDA_CON0, 0, 10, ch);
    if (AUD_CH_VCM == aud2adc_ch) {
        SFR(JL_ADDA->ADDA_CON0, 21, 1, 1);
    }
    SFR(JL_ADDA->ADDA_CON0, 0, 1, 1);

}


/*----------------------------------------------------------------------------*/
/**@brief   audio 相关电源测试通路关闭函数
   @return  无
   @author  liuhaokun
   @note    void audio2saradc_ch_close(void)
*/
/*----------------------------------------------------------------------------*/
void audio2saradc_ch_close(void)
{
    SFR(JL_ADDA->ADDA_CON0, 0, 1, 0);
    SFR(JL_ADDA->ADDA_CON0, 2, 8, 0);
    SFR(JL_ADDA->ADDA_CON0, 21, 1, 0);

}


