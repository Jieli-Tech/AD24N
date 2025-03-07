#include "config.h"
#include "audio.h"
#include "audio_adc.h"
#include "audio_cpu.h"
#include "audio_adc_cpu.h"
#include "audio_apa_cpu.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[auadc_cpu]"
#include "log.h"

extern const bool audio_adc_diff_mic_mode; // 差分amic使能(N端固定PA15) 0:单端amic  1:差分amic
extern const bool audio_adc_diff_aux_mode; // 差分aux使能(N端固定PA15) 0:单端aux  1:差分aux

extern const AUADC_PERFORM_MODE    auadc_perf_mode;
extern const AUDIO_MIC_RS_MODE     auadc_mic_rs_pwr_mode;//
extern const AUDIO_MICPGA_G        audio_adc_mic_pga_g;//后级增益

u32 audio_adc_buf[AUDIO_ADC_PACKET_SIZE * AUDIO_ADC_SP_SIZE  / 4 ] ;//AT(.AUDIO_ADC_BUFFER);

AUDIO_ADC_CTRL_HDL c_audio_adc_hdl = {
    .buf      = (void *) &audio_adc_buf[0],
    .pns      = AUDIO_ADC_SP_PNS,
    .sp_total = sizeof(audio_adc_buf) / AUDIO_ADC_SP_SIZE,
    .sp_size  = AUDIO_ADC_SP_SIZE,
    .throw    = 25 * 32,
};

const  AUADC_SFR c_aumic_rs_in_sfr  = {
    .ada_con0 = AUADC_RS_IN_CON0,
    .ada_con2 = AUADC_RS_IN_CON2,
    /* .ada_con3 = 0, */
    .ada_con4 = AUADC_RS_IN_CON4,
};
const  AUADC_SFR c_aumic_rs_out_sfr  = {
    .ada_con0 = AUADC_RS_OUT_CON0,
    .ada_con2 = AUADC_RS_OUT_CON2,
    /* .ada_con3 = 0, */
    .ada_con4 = AUADC_RS_OUT_CON4,
};

const  AUADC_POWER_SFR c_auadc_power_low_sfr  = {
    .ada_con0 = AUADC_POWER_LOW_CON0,
    .ada_con2 = AUADC_POWER_LOW_CON2,
};
const  AUADC_POWER_SFR c_auadc_power_standaed_sfr  = {
    .ada_con0 = AUADC_POWER_NORMAL_CON0,
    .ada_con2 = AUADC_POWER_NORMAL_CON2,
};


u8 const audio_adc_dmic_use_ch = 4; // adc0 mic sel  0~3:不使用数字麦  4:dmic0上升沿 5:dmic0下降沿 6:dmic1上升沿 7:dmic1下降沿
AUDIO_ADC_DIG_MIC_HDL audio_adc_dmic_hdl = {
    .clk_io = IO_PORTA_10, //支持output_channel映射
    .data_io = IO_PORTA_11, //支持output_channel映射
    .data_ch = PORT_FUNC_ICH_PLNK_DAT0,//对应数字麦0
    /* .data_ch = PORT_FUNC_ICH_PLNK_DAT1,//对应数字麦1 */
    /* .enable = 0, */
};


#define AUADC_GPIO_SET_HIGHZ(port,num) \
            JL_PORT##port->DIR |=  BIT(num);  \
            JL_PORT##port->DIE &= ~BIT(num);  \
            JL_PORT##port->PU0 &= ~BIT(num);  \
            JL_PORT##port->PU1 &= ~BIT(num);  \
            JL_PORT##port->PD0 &= ~BIT(num);  \
            JL_PORT##port->PD1 &= ~BIT(num);


void auin_mode_init(void)
{
    fadc_init((void *)&c_audio_adc_hdl);
    HWI_Install(IRQ_ADC_SOFT, (u32)audio_adc_isr, IRQ_AUADC_IP);
}

/*----------------------------------------------------------------------------*/
/**@brief   开启LINE_IN/模拟麦的输入通路
   @param   mode: ADC应用模式，该函数仅支持ADC_LINE_IN和ADC_MIC(模拟麦)
            port: ADC输入引脚
            is_diff: 是否使用差分输入
   @return  0：成功；
            error:失败
   @author  mawancheng
   @note    static u32 audio_amic_open(ADC_MODE mode, AUDIO_MIC_INPUT_PORT port)
*/
/*----------------------------------------------------------------------------*/
static u32 audio_amic_open(ADC_MODE mode, AUDIO_MIC_INPUT_PORT port)
{

    const AUADC_SFR *p_sfr = NULL;
    bool is_diff = 0;
    audio_adc_analog_close();
    /* JL_ADDA->ADA_CON0 = 0;  */
    /* JL_ADDA->ADA_CON1 = 0;  */
    /* JL_ADDA->ADA_CON2 = 0;  */
    /* JL_ADDA->ADA_CON3 = 0;  */
    /* JL_ADDA->ADA_CON4 = 0; */

    switch (mode) {
    case ADC_MIC:
        is_diff = audio_adc_diff_mic_mode;
        if (MIC_RS_INSIDE == auadc_mic_rs_pwr_mode) {
            p_sfr = &c_aumic_rs_in_sfr;
            AUADC_GPIO_SET_HIGHZ(A, 13);
        } else if (MIC_RS_OUTSIDE == auadc_mic_rs_pwr_mode) {
            p_sfr = &c_aumic_rs_out_sfr;
            AUADC_GPIO_SET_HIGHZ(A, 13);
        }
        goto __ADC_MIC_AND_LINEIN;
    case ADC_LINE_IN:
        is_diff = audio_adc_diff_aux_mode;
__ADC_MIC_AND_LINEIN:
        if (is_diff) {
            AUADC_GPIO_SET_HIGHZ(A, 15); //将固定的N端初始化为模拟输入
            SFR(JL_ADDA->ADA_CON0,   21, 1,  1);     //AUDADC_MICA_DIFF_EN_11v
        } else {
            /* 单端 */
            SFR(JL_ADDA->ADA_CON4,   8, 1,  1);      //AUDADC_BUFFER_CHOP_EN_11v
            SFR(JL_ADDA->ADA_CON0,   18, 1,  1);     //AUDADC_MICA_BUFN_EN_11v
        }
        break;
    case ADC_MIC_APA :
        port = MIC_INPUT_ANA3_APAP;
        break;
    case DIGITAL_MIC:
    default:
        return E_AUADC_NULL;
    }

    switch (port) {
    case MIC_INPUT_ANA0_PA13:
        /* JL_ADDA->ADA_CON0 |= BIT(9);  //A0通路打开 */
        SFR(JL_ADDA->ADA_CON0, 9, 4, 1 << 0);
        AUADC_GPIO_SET_HIGHZ(A, 13);
        break;
    case MIC_INPUT_ANA1_PB1:
        /* JL_ADDA->ADA_CON0 |= BIT(10); //A1通路打开 */
        SFR(JL_ADDA->ADA_CON0, 9, 4, 1 << 1);
        AUADC_GPIO_SET_HIGHZ(B, 1);
        break;
    case MIC_INPUT_ANA2_PA14:
        /* JL_ADDA->ADA_CON0 |= BIT(11); //A2通路打开 */
        SFR(JL_ADDA->ADA_CON0, 9, 4, 1 << 2);
        AUADC_GPIO_SET_HIGHZ(A, 14);
        break;
    case MIC_INPUT_ANA3_APAP:
        if (!is_apa_close_clear()) {
            return E_ADCANA_PARA;
        }
        apa_n_highz();
        apa_p_highz();
        /* JL_ADDA->ADA_CON0 |= BIT(12); //A3通路打开 */
        SFR(JL_ADDA->ADA_CON0, 9, 4, 1 << 3);
        SFR(JL_ADDA->ADA_CON0,   22, 1,  1);     //AUDADC_MICA_DIFF1_EN_11v
        SFR(JL_ADDA->ADA_CON0,   18, 1,  0);     //AUDADC_MICA_BUFN_EN_11v
        break;
    }

__amic_open_end:
    const AUADC_POWER_SFR *power;
    if (AUADC_STANDARD_PERF == auadc_perf_mode) { //audio_adc性能i
        power = &c_auadc_power_standaed_sfr;
    } else {
        power = &c_auadc_power_low_sfr;
    }
    audio_adc_analog_init(p_sfr,  power, audio_adc_mic_pga_g);
    log_info("amic_open_end\n");
    SFR(JL_AUD->AUD_CON1,  8,  4,  0);                  // adc0 mic sel  0: amic0  1:*  2:* 3:*  4:dmic0 5: dmic1 6: dmic2 7: dmic3
    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   设置数字麦使用的IO和crossbar
   @param   clk_io: 数字麦时钟IO，如IO_PORTA_10，支持映射
            data_io: 数字麦数据输入IO，如IO_PORTA_11，支持映射
            use_ich: 数字麦数据输入通路，仅支持PORT_FUNC_ICH_PLNK_DAT0和PORT_FUNC_ICH_PLNK_DAT1
   @return  0：成功；
   @author  mawancheng
   @note    static u32 audio_dmic_port_init(u32 clk_io, u32 data_io, u32 use_ich)

*/
/*----------------------------------------------------------------------------*/
static u32 audio_dmic_port_init(AUDIO_ADC_DIG_MIC_HDL *p_auadc_dmic_hdl)
{
    if (1 == p_auadc_dmic_hdl->enable) {
        log_error("dmic_port_already_enable");
        return -1;
    }

    gpio_set_function(IO_PORT_SPILT(p_auadc_dmic_hdl->clk_io), PORT_FUNC_OCH_PLNK_CLK);
    gpio_hw_set_direction(IO_PORT_SPILT(p_auadc_dmic_hdl->clk_io), 0);

    gpio_set_function(IO_PORT_SPILT(p_auadc_dmic_hdl->data_io), p_auadc_dmic_hdl->data_ch);
    gpio_hw_set_direction(IO_PORT_SPILT(p_auadc_dmic_hdl->data_io), 1);
    gpio_hw_set_die(IO_PORT_SPILT(p_auadc_dmic_hdl->data_io), 1);
    p_auadc_dmic_hdl->enable = 1;
    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   释放数字麦使用的IO和crossbar
   @param   clk_io: 数字麦时钟IO，如IO_PORTA_10
            data_io: 数字麦数据输入IO，如IO_PORTA_11
            use_ich: 数字麦数据输入通路，仅支持PORT_FUNC_ICH_PLNK_DAT0和PORT_FUNC_ICH_PLNK_DAT1
   @return  0：成功；
   @author  mawancheng
   @note    static u32 audio_dmic_port_uninit(u32 clk_io, u32 data_io, u32 use_ich)

*/
/*----------------------------------------------------------------------------*/
static u32 audio_dmic_port_uninit(AUDIO_ADC_DIG_MIC_HDL *p_auadc_dmic_hdl)
{
    if (0 == p_auadc_dmic_hdl->enable) {
        /* log_error("dmic_port_not_using\n"); */
        return -1;
    }
    gpio_disable_function(IO_PORT_SPILT(p_auadc_dmic_hdl->clk_io), PORT_FUNC_OCH_PLNK_CLK);
    gpio_disable_function(IO_PORT_SPILT(p_auadc_dmic_hdl->data_io), p_auadc_dmic_hdl->data_ch);
    p_auadc_dmic_hdl->enable = 0;
    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   初始化Audio_ADC模块
   @param   sr: 采样率
            mode: ADC应用模式。可选择ADC_LINE_IN、ADC_MIC(模拟麦)、DIGITAL_MIC(数字麦)
            ch: LINE_IN和模拟麦的输入通路；使用数字麦时，该参数无效
   @return  0：成功；
            error：失败，具体根据错误码排查原因
   @author  mawancheng
   @note    u32 auin_init(u32 sr, ADC_MODE mode, u32 ch)
*/
/*----------------------------------------------------------------------------*/
u32 auin_init(u32 sr, ADC_MODE mode, u32 ch)
{
    if (sr == 0) {
        return E_ADC_SR;
    }

    auadc_clk_open();

    u32 res = 0;
    if (DIGITAL_MIC == mode) {
        au_dmic_clk_open();
        res = audio_dmic_port_init(&audio_adc_dmic_hdl);
        // adc0 mic sel  0: amic0  1:*  2:* 3:*  4:dmic0 5: dmic1 6: dmic2 7: dmic3
        SFR(JL_AUD->AUD_CON1,  8,  4,  audio_adc_dmic_use_ch);
    } else {
        res = audio_amic_open(mode, ch);
    }
    if (res) {
        return res;
    }
    res = audio_adc_phy_init(sr);

    return res;
}


/*----------------------------------------------------------------------------*/
/**@brief   使能并启动Audio_ADC模块，需要在ADC初始化后调用
   @param   gain: 无效参数
   @return  void
   @author  mawancheng
   @note    void audio_adc_enable(u32 gain)
*/
/*----------------------------------------------------------------------------*/
void audio_adc_enable(u32 gain)
{
    audio_adc_enable_phy();
}

/*----------------------------------------------------------------------------*/
/**@brief   关闭Audio_ADC模块
   @param   void
   @return  void
   @author  mawancheng
   @note    void auin_off_api(void)

*/
/*----------------------------------------------------------------------------*/
void auin_off_api(void)
{
    audio_adc_phy_off();
    audio_dmic_port_uninit(&audio_adc_dmic_hdl);
    au_dmic_clk_close();
    audio_adc_analog_close();
}

void dump_audio_adc_sfr(void)
{
    log_info("JL_LSBCLK->PRPCON1  = %8xH", JL_LSBCLK->PRP_CON1);
    log_info("JL_AUD->AUD_CON0    = %8xH", JL_AUD->AUD_CON0);
    log_info("JL_AUD->AUD_CON1    = %8xH", JL_AUD->AUD_CON1);
    log_info("JL_AUD->AUD_COP     = %8xH", JL_AUD->AUD_COP);
    log_info("JL_ADDA->ADDA_CON0  = %8xH", JL_ADDA->ADDA_CON0);
    log_info("JL_ADDA->ADDA_CON1  = %8xH", JL_ADDA->ADDA_CON1);
    log_info("JL_ADDA->ADA_CON0  = %8xH", JL_ADDA->ADA_CON0);
    log_info("JL_ADDA->ADA_CON1  = %8xH", JL_ADDA->ADA_CON1);
    log_info("JL_ADDA->ADA_CON2  = %8xH", JL_ADDA->ADA_CON2);
    log_info("JL_ADDA->ADA_CON3  = %8xH", JL_ADDA->ADA_CON3);
    log_info("JL_ADDA->ADA_CON4  = %8xH", JL_ADDA->ADA_CON4);
    log_info("JL_AUDADC->ADC_CON0 = %8xH", JL_AUDADC->ADC_CON0);
    log_info("JL_AUDADC->ADC_CON1 = %8xH", JL_AUDADC->ADC_CON1);
    log_info("JL_AUDADC->ADC_COP  = %8xH", JL_AUDADC->ADC_COP);

    log_info("\n");
    log_info("JL_AUDADC->ADC_ADR = %8xH", JL_AUDADC->ADC_ADR);
    log_info("JL_AUDADC->ADC_LEN = %8xH", JL_AUDADC->ADC_LEN);
    log_info("JL_AUDADC->ADC_PNS = %8xH", JL_AUDADC->ADC_PNS);

    log_info("\n");
}

#if 0
void audio_adc_test_demo(void)
{
    log_info("audio_adc_test_demo\n");

    audio_init();

    audio_adc_init_api(32000, ADC_MIC, audio_adc_mic_input_port);

    audio_adc_enable(1);

    log_info("audio_adc_init_over\n");
    dump_audio_adc_sfr();

    while (1) {
        wdt_clear();
    }

}
#endif



