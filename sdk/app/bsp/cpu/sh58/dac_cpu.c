/***********************************Jieli tech************************************************
  File : dac_api.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2019-1-14
********************************************************************************************/
/* #include "rdac.h" */
#include "dac_api.h"
#include "dac.h"
#include "config.h"
#include "audio.h"
#include "audio_analog.h"
#include "audio_dac_cpu.h"
#include "audio_apa_cpu.h"
/* #include "asm/power_interface.h" */

/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[dac cpu]"
#include "log.h"


static u16 audio_dac_buf[DAC_PACKET_SIZE * 2];

#define DAC_MAX_SP  (sizeof(audio_dac_buf) / 2)
#define DAC_PNS     (DAC_MAX_SP / 3)

DAC_CTRL_HDL audio_dac_ops = {
    .buf         = audio_dac_buf,
    .sp_total    = DAC_MAX_SP,
    .sp_max_free = (DAC_MAX_SP - DAC_PNS) / 2,
    /* .sp_size     = DAC_TRACK_NUMBER * (AUDAC_BIT_WIDE / 8), */
    .con0        = DAC_CON0_DEFAULT,
    .con1        = DAC_CON1_DEFAULT,
    .pns         = DAC_PNS,
};

void audio_dac_para_init(void)
{
    memset(&audio_dac_buf[0], 0, sizeof(audio_dac_buf));
    fdac_resource_init((void *)&audio_dac_ops);
}



/* void audac_test_tab_init(void); */
void dump_audio_sfr(void);

void audio_dac_init(u32 sr, bool delay_flag)
{
    /* rdac_analog_open(); */
    /* delay(1000); */

    adda_clk_open(sr);
    apa_clk_sel();
    if (0 != au_const_dac_analog_en) {
        adda_dac_analog_init();
    }

    HWI_Install(IRQ_DAC_SOFT, (u32)dac_isr, IRQ_AUDAC_IP) ;
    /* audio_dac_analog_init(); */
    /* JL_AUD->AUD_COP = 63; //ADDA公共配置，移到audio_commom_analog */
    dac_phy_init(dac_sr_lookup(sr));

    if (au_const_dac_digital_en) {
        SFR(JL_AUD->AUD_CON0,  3,  1,  1);
    } else {
        SFR(JL_AUD->AUD_CON0,  3,  1,  0);
    }
    /* rdac_trim_api(); */
    JL_AUDDAC->DAC_CON0 |= DAC0_RELEASE_RST;

    if (au_const_apa_en) {
        apa_init(sr, 1);
    }

    /* audac_test_tab_init(); */
    /* log_info("audio dac test demo"); */
    dump_audio_sfr();
}

void audio_dac_sr_api(u32 sr)
{
    dac_sr_set(dac_sr_lookup(sr));
}


void audio_dac_off_api(void)
{
    dac_phy_off();
    adda_dac_analog_close();
    apa_close();

}

/*
void rdac_trim_api(void)
{
    u32 dac_trim_val = 0;
    JL_AUDIO->DAC_TM0 = dac_trim_val;
    delay(2000);
}
*/

/*
void audio_config_hexdump(void)
{
    log_info("AUDIO CLOCK");
    log_info("JL_ASS->CLK_CON   : 0x%x", JL_ASS->CLK_CON);
    log_info("JL_CLOCK->PRP_CON0: 0x%x", JL_CLOCK->PRP_CON0);
    log_info("JL_CLOCK->PRP_TCON: 0x%x\n", JL_CLOCK->PRP_TCON);

    log_info("AUDIO POWER");
    log_info("JL_ADDA->AVDD_CON0: 0x%x", JL_ADDA->AVDD_CON0);
    log_info("JL_ADDA->AVDD_CON1: 0x%x", JL_ADDA->AVDD_CON1);
    log_info("P3_CHG_PUMP       : 0x%x",   P33_CON_GET(P3_CHG_PUMP));
    log_info("P3_ANA_FLOW1      : 0x%x\n", P33_CON_GET(P3_ANA_FLOW1));

    log_info("AUDIO DIGITAL");
    log_info("JL_AUDIO->DAC_CON : 0x%x", JL_AUDIO->DAC_CON);
    log_info("JL_AUDIO->DAC_CON1: 0x%x", JL_AUDIO->DAC_CON1);
    log_info("JL_AUDIO->DAC_CON2: 0x%x", JL_AUDIO->DAC_CON2);
    log_info("JL_AUDIO->DAC_CON3: 0x%x", JL_AUDIO->DAC_CON3);
    log_info("JL_AUDIO->DAC_CON4: 0x%x", JL_AUDIO->DAC_CON4);
    log_info("JL_AUDIO->AUD_CON : 0x%x", JL_AUDIO->AUD_CON);
    log_info("JL_AUDIO->DAC_VL0 : 0x%x", JL_AUDIO->DAC_VL0);
    log_info("JL_AUDIO->DAC_DIT : 0x%x", JL_AUDIO->DAC_DIT);
    log_info("JL_AUDIO->DAC_TM0 : 0x%x", JL_AUDIO->DAC_TM0);
    log_info("JL_AUDIO->DAC_COP : 0x%x\n", JL_AUDIO->DAC_COP);

    log_info("AUDIO ANALOG");
    log_info("JL_ADDA->DAA_CON0 : 0x%x", JL_ADDA->DAA_CON0);
    log_info("JL_ADDA->DAA_CON7 : 0x%x", JL_ADDA->DAA_CON7);
    log_info("JL_ADDA->HADA_CON0: 0x%x", JL_ADDA->HADA_CON0);
    log_info("JL_ADDA->HADA_CON1: 0x%x", JL_ADDA->HADA_CON1);
    log_info("JL_ADDA->HADA_CON2: 0x%x", JL_ADDA->HADA_CON2);
    log_info("JL_ADDA->ADDA_CON0: 0x%x", JL_ADDA->ADDA_CON0);
    log_info("JL_ADDA->AVDD_CON0: 0x%x", JL_ADDA->AVDD_CON0);
    log_info("JL_ADDA->AVDD_CON1: 0x%x\n", JL_ADDA->AVDD_CON1);
}
*/

void dump_audio_sfr(void)
{
    log_info("JL_LSBCLK->PRPCON1  = %8xH", JL_LSBCLK->PRP_CON1);
    log_info("JL_AUD->AUD_CON0    = %8xH", JL_AUD->AUD_CON0);
    log_info("JL_AUD->AUD_CON1    = %8xH", JL_AUD->AUD_CON1);
    log_info("JL_AUD->AUD_COP     = %8xH", JL_AUD->AUD_COP);
    log_info("JL_ADDA->ADDA_CON1  = %8xH", JL_ADDA->ADDA_CON1);
    log_info("JL_ADDA->DAA_CON1   = %8xH", JL_ADDA->DAA_CON1);
    log_info("JL_ADDA->DAA_CON2   = %8xH", JL_ADDA->DAA_CON2);
    log_info("JL_AUDDAC->DAC_CON0 = %8xH", JL_AUDDAC->DAC_CON0);
    log_info("JL_AUDDAC->DAC_CON1 = %8xH", JL_AUDDAC->DAC_CON1);
    log_info("JL_AUDDAC->DAC_CON2 = %8xH", JL_AUDDAC->DAC_CON2);
    /* log_info("JL_AUDADC->ADC_COP  = %8xH", JL_AUDADC->ADC_COP); */

    log_info("\n");
    log_info("JL_AUDDAC->DAC_ADR = %8xH", JL_AUDDAC->DAC_ADR);
    log_info("JL_AUDDAC->DAC_LEN = %8xH", JL_AUDDAC->DAC_LEN);
    /* log_info("JL_AUDDAC->DAC_PNS = %8xH", JL_AUDDAC->DAC_PNS); */
    log_info("JL_AUDDAC->DAC_HRP = %8xH", JL_AUDDAC->DAC_HRP);
    log_info("JL_AUDDAC->DAC_VOL = %8xH", JL_AUDDAC->DAC_VOL);
    log_info("JL_AUDDAC->DAC_DIT = %8xH", JL_AUDDAC->DAC_DIT);
    log_info("JL_AUDDAC->DAC_TM0 = %8xH", JL_AUDDAC->DAC_TM0);

    log_info("\n");
    log_info("JL_LSBCLK->PRP_CON1 = %8xH", JL_LSBCLK->PRP_CON1);
    log_info("JL_APA->APA_CON3    = %8xH", JL_APA->APA_CON3);
    log_info("JL_APA->APA_CON0    = %8xH", JL_APA->APA_CON0);
    log_info("JL_APA->APA_CON1    = %8xH", JL_APA->APA_CON1);
    log_info("JL_APA->APA_CON2    = %8xH", JL_APA->APA_CON2);
    log_info("JL_APA->APA_CON4    = %8xH", JL_APA->APA_CON4);
    log_info("JL_APA->APA_CON5    = %8xH", JL_APA->APA_CON5);
}

#if 0

#include "tab_read.h"
rtab_obj sine_wav_t_obj;
static const u16 sine_wav_t[] = {
    0x0000,
    0xa57e,
    0x8000,
    0xa57e,
    0x0000,
    0x5a82,
    0x7fff,
    0x5a82
};

void audac_test_tab_init(void)
{
    tab_init(&sine_wav_t_obj, (void *)sine_wav_t, sizeof(sine_wav_t));
    sine_wav_t_obj.cnt = -1;
}
s16 data_t = 0;
void audac_test_tab_read(void *buff, u32 len)
{
    tab_read(buff, &sine_wav_t_obj, len);
    return;
    /* s16 *p = (void *)buff; */
    /* u32 s16len = len / 2; */
    /* for (u32 i = 0; i < s16len; i++) { */
    /* p[i] = data_t; */
    /* data_t += 1000; */
    /* s16len--; */
    /* } */

}



void audio_dac_test_demo(void)
{
    audac_test_tab_init();
    log_info("audio dac test demo");
#if 0
    audio_dac_mode_init();
    audio_init();
    /* adda_clk_open(2000); */
    /* apa_clk_sel(); */
    rdac_phy_vol(16383);
    audio_dac_init(48000, 0);
#endif


    log_info("audio dac init over");
    dump_audio_sfr();

    while (1) {
        wdt_clear();
    }


}
#endif
