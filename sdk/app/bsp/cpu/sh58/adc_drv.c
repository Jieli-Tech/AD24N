#include "asm/power_interface.h"
#include "gpio.h"
#include "adc_drv.h"
#include "cpu.h"
#include "hwi.h"
#include "clock.h"
#include "audio_signal.h"
#include "errno-base.h"

#define LOG_TAG_CONST         NORM
#define LOG_TAG               "[saradc]"
#include "log.h"

#define MVBG_TEST_SEL(ch)       P33_CON_SET(P3_PMU_ADC0, 4, 2, ch)
#define VBG_TEST_EN(en)      	p33_fast_access(P3_PMU_ADC0, BIT(3), en)
#define VBG_BUFFER_EN(en)      	p33_fast_access(P3_PMU_ADC0, BIT(2), en)
#define PMU_DET_OE(en)      	p33_fast_access(P3_PMU_ADC0, BIT(1), en)
#define PMU_TOADC_EN(en)      	p33_fast_access(P3_PMU_ADC0, BIT(0), en)
#define ADC_CHANNEL_SEL(ch)     P33_CON_SET(P3_PMU_ADC1, 0, 4, ch)

#define B_SADC_ISR_READY                BIT(0)
#define B_SADC_BUSY                     BIT(1)
#define B_SADC_SOMEONE_WAS_WAITTING     BIT(2)
void udelay(u32 us);



typedef struct _adc_info {
    u16 value;
    u8 ch;
} adc_info_t;

void (*adc_scan_over_cb)(void) = NULL;
static adc_info_t adc_info[ADC_MAX_CH_NUM];
static volatile u8 cur_ch = 0;
volatile u8 adc_flag = 0;
static u16 vbg_value = 0;

const u8 adc_clk_div_t[] = {
    1,     /*000*/
    6,     /*001*/
    12,    /*010*/
    24,    /*011*/
    48,    /*100*/
    72,    /*101*/
    96,    /*110*/
    128,    /*111*/
};
#define ADC_MAX_CLK    1000000L

u8 adc_ch2port(u16 real_ch)
{
    /* if (real_ch > ADC_CH_PC7) { */
    /*     return IO_PORT_MAX; */
    /* } */
    u32 ch = real_ch & 0xf;
    const u8 io_adcch_map_table[] = {
        IO_PORTA_00,
        IO_PORTA_01,
        IO_PORTA_02,
        IO_PORTA_03,
        IO_PORT_DP,
        IO_PORT_DM,
        IO_PORTA_04,
        IO_PORTA_05,
        IO_PORTA_06,
        IO_PORTA_10,
        IO_PORTA_11,
        IO_PORTA_12,
        IO_PORTA_13,
        IO_PORTA_14,
        IO_PORTA_15,
        IO_PORTB_00,
    };

    return io_adcch_map_table[ch];
}

static u32 adc_is_wait(void)
{
    local_irq_disable();
    if (adc_flag & B_SADC_SOMEONE_WAS_WAITTING) {
        local_irq_enable();
        return E_ADC_WAIT;
    }
    adc_flag |= B_SADC_SOMEONE_WAS_WAITTING;
    local_irq_enable();
    return 0;
}

static u32 adc_is_busy(u32 retry)
{
    while (1) {
        if (!(adc_flag & B_SADC_BUSY)) {
            break;
        }
        if (0 == (retry--)) {
            return E_ADC_BUF;
        }
    }
    return 0;
}

static u32 adc_idle(u32 retry)
{
    u32 err = adc_is_wait();
    if (0 != err) {
        /* putchar('w'); */
        return err;
    }
    err = adc_is_busy(retry);
    if (0 != err) {
        adc_flag &= ~B_SADC_SOMEONE_WAS_WAITTING;
        /* putchar('b'); */
        return err;
    }
    adc_flag |= B_SADC_BUSY;
    adc_flag &= ~B_SADC_SOMEONE_WAS_WAITTING;
    return 0;
}

u32 adc_add_sample_ch(u16 real_ch)
{
    u32 res = 0;
    local_irq_disable();
    for (u8 i = 0; i < ADC_MAX_CH_NUM; i++) {
        if ((adc_info[i].ch & ADC_MASK_CHANNEL_SEL) == real_ch) {
            adc_info[i].value = ADC_VALUE_NONE;
            res = E_SADC_WAS_HAD_CH;
            break;
        } else if (adc_info[i].ch == 0xff) {
            adc_info[i].ch = real_ch;
            adc_info[i].value = ADC_VALUE_NONE;
            log_info("add sample ch %x, chidx %d\n", real_ch, i);
            res = 0;
            break;
        }
    }
    local_irq_enable();
    return res;
}

void adc_remove_sample_ch(u16 real_ch)
{
    local_irq_disable();
    for (u8 i = 0; i < ADC_MAX_CH_NUM; i++) {
        if ((adc_info[i].ch & ADC_MASK_CHANNEL_SEL) == real_ch) {
            adc_info[i].ch = 0xff;
            adc_info[i].value = ADC_VALUE_NONE;
            break;
        }
    }
    local_irq_enable();
}

u8 adc_add_ch_reuse(u16 real_ch, u8 busy)
{
    for (u8 i = 0; i < ADC_MAX_CH_NUM; i++) {
        if ((adc_info[i].ch & ADC_MASK_CHANNEL_SEL) == real_ch) {
            if (busy) {
                adc_info[i].ch |= ADC_IO_CAN_NOT_USE;
                adc_info[i].value = ADC_VALUE_NONE;
            } else {
                adc_info[i].ch &= ~ADC_IO_CAN_NOT_USE;
            }
        }
    }
    return -1;
}

u8 adc_remove_ch_reuse(u16 real_ch)
{
    for (u8 i = 0; i < ADC_MAX_CH_NUM; i++) {
        if ((adc_info[i].ch & ADC_MASK_CHANNEL_SEL) == real_ch) {
            adc_info[i].ch &= ~ADC_IO_CAN_NOT_USE;
        }
    }
    return -1;
}

static u8 adc_get_next_ch(u8 now_ch)
{
    for (u8 i = now_ch + 1; i < ADC_MAX_CH_NUM; i++) {
        if (adc_info[i].ch != 0xff) {
            if (0 != (adc_info[i].ch & ADC_IO_CAN_NOT_USE)) {
                continue;
            }
            return i;
        }
    }
    return 0xff;
}

SET(interrupt(""))
void adc_isr()
{
    JL_ADC->CON |= BIT(ADC_CON_DONE_PND_CLR);
    adc_info[cur_ch].value = JL_ADC->RES;

    PMU_DET_OE(0);
    PMU_TOADC_EN(0);
    /* putchar('t'); */
    adc_scan();


}

static u32 adc_audio_ch_select(u16 ch)
{
    __AUD_CH aud2adc_ch = 0;
    __AUD_CH_OTHER aud2adc_ch_other = 0;

    if (ADC_AUDIO_SUB & ch) {
        //一级通道选择
        u32 tmp = ch & 0x7;
        if ((tmp > 5) || (tmp < 1)) {
            log_error("do not have current audio_ch 0x%x\n", ch);
            return E_SADC_CH_NULL;
        }
        aud2adc_ch = tmp;
    } else {
        aud2adc_ch = AUD_CH_OTHER;
        aud2adc_ch_other = (ch & 0x7);
        if (aud2adc_ch_other > 5) {
            log_error("do not have current audio_ch 0x%x\n", ch);
            return E_SADC_CH_NULL;
        }
    }
    audio2saradc_ch_open(aud2adc_ch, aud2adc_ch_other);
    return 0;
}

static u32 adc_sample(u16 real_ch, bool isr_flag)
{
    u8 time_div = 0;
    u32 ret = 0;
    JL_ADC->CON = BIT(ADC_CON_DONE_PND_CLR) | BIT(ADC_CON_CPND);
    int clk = clk_get("lsb");
    for (int j = 0; j < sizeof(adc_clk_div_t) / sizeof(adc_clk_div_t[0]); j++) {
        if (clk / adc_clk_div_t[j] <= ADC_MAX_CLK) {
            time_div = adc_clk_div_t[j];
            time_div = time_div / 2;
            /* log_info("time_div 0x%x\n",time_div); */
            break;
        }
    }
    /* SFR(JL_ADC->CON, ADC_CON_ADC_BAUD, ADC_CON_ADC_BAUD_, time_div); */
    SFR(JL_ADC->CON, ADC_CON_ADC_BAUD, ADC_CON_ADC_BAUD_, 0b111);
    SFR(JL_ADC->CON, ADC_CON_WAIT_TIME, ADC_CON_WAIT_TIME_, 0);//延时启动，实际启动延时为N*8个CLK
    /* 通道选择 */
    if ((real_ch & 0x30) == ADC_MUX_IO) {
        /* log_info("IO\n"); */
        SFR(JL_ADC->CON, ADC_CON_CH_SEL, ADC_CON_CH_SEL_, real_ch & 0xf);
        SFR(JL_ADC->CON, ADC_CON_ADC_MUX_SEL, ADC_CON_ADC_MUX_SEL_, 0b001);
    } else if ((real_ch & 0x30) == ADC_MUX_AN) {
        if (real_ch == ADC_CH_SYSPLL_LDO) {//增加syspll_ldo模式
            SFR(JL_ADC->CON, ADC_CON_ADC_ASEL, ADC_CON_ADC_ASEL_, ADC_SFR_SYSPLL);
            SFR(JL_ADC->CON, ADC_CON_ADC_MUX_SEL, ADC_CON_ADC_MUX_SEL_, 0b010);
        } else {
            ADC_CHANNEL_SEL(real_ch & 0xf);//CHANNEL
            if (real_ch == ADC_CH_PMU_VBG) {
                /* log_info("VBG\n"); */
                MVBG_TEST_SEL(0b01);
            }
            PMU_DET_OE(1);
            PMU_TOADC_EN(1);
            SFR(JL_ADC->CON, ADC_CON_ADC_ASEL, ADC_CON_ADC_ASEL_, ADC_SFR_PMU);
            SFR(JL_ADC->CON, ADC_CON_ADC_MUX_SEL, ADC_CON_ADC_MUX_SEL_, 0b010);
        }
    } else if ((real_ch & 0x30) == ADC_AUDIO_AN) {
        SFR(JL_ADC->CON, ADC_CON_ADC_ASEL, ADC_CON_ADC_ASEL_, ADC_SFR_AUDIO);
        SFR(JL_ADC->CON, ADC_CON_ADC_MUX_SEL, ADC_CON_ADC_MUX_SEL_, 0b010);
        ret = adc_audio_ch_select(real_ch);
        if (ret) {
            return ret;
        }
    } else if ((real_ch & 0x30) == ADC_CLASSD_AN) {
        SFR(JL_ADC->CON, ADC_CON_ADC_ASEL, ADC_CON_ADC_ASEL_, ADC_SFR_CLASSD);
        SFR(JL_ADC->CON, ADC_CON_ADC_MUX_SEL, ADC_CON_ADC_MUX_SEL_, 0b010);
    } else {
        log_error("channle error\n");
        return E_SADC_CH_TYPE;
    }
    JL_ADC->CON |= BIT(ADC_CON_ADC_EN) | BIT(ADC_CON_ADC_AE) | BIT(ADC_CON_ADC_CLKEN);
    if (isr_flag) {
        if (adc_flag & B_SADC_ISR_READY) {
            JL_ADC->CON |= BIT(ADC_CON_DONE_PND_IE);  //IE
        } else {
            return E_SADC_ISR_NOT_READY;
        }
    } else {
        JL_ADC->CON &= ~BIT(ADC_CON_DONE_PND_IE);
    }
    JL_ADC->CON |= BIT(ADC_CON_CPND);
    return 0;
}

int adc_kick_start(void (*adc_scan_over)(void))
{
    u32 err = adc_idle(0);
    if (0 != err) {
        /* putchar('e'); */
        return err;
    }
    u8 ch = adc_get_first_available_ch();
    if (ch == 0xff) {
        log_error("kick start find channle error\n");
        return -1;
    }
    adc_scan_over_cb = adc_scan_over;
    /* putchar('k'); */
    return adc_sample(adc_info[cur_ch].ch, 1);
}


u16 adc_get_first_available_ch()
{
    for (u8 i = 0; i < ARRAY_SIZE(adc_info); i++) {
        if (adc_info[i].ch != 0xff) {
            if (0 != (adc_info[i].ch & ADC_IO_CAN_NOT_USE))  {
                continue;
            }
            cur_ch = i;
            return cur_ch;
        }
    }
    return 0xff;
}

void adc_scan(void)
{
    cur_ch = adc_get_next_ch(cur_ch);
    if (cur_ch == 0xff) {
        /* ADC_DEN(0); */
        /* IO_CHL_EN(0); */
        /* TEST_CHL_EN(0); */
        /* PMU_DET_EN(0); */
        /* putchar('o'); */
        adc_flag &= ~B_SADC_BUSY;
        if (adc_scan_over_cb) {
            adc_scan_over_cb();
        }
        return;
    }
    adc_sample(adc_info[cur_ch].ch, 1);
}

u16 adc_get_value(u16 real_ch)
{
    for (u8 i = 0; i < ARRAY_SIZE(adc_info); i++) {
        if ((adc_info[i].ch & ADC_MASK_CHANNEL_SEL) == real_ch) {
            return adc_info[i].value;
        }
    }

    return ((u16) - 1);
}

void adc_isr_init(void)
{
    local_irq_disable();
    for (int i = 0; i < ADC_MAX_CH_NUM; i++) {
        adc_info[i].ch = 0xff;
        adc_info[i].value = ADC_VALUE_NONE;
    }
    HWI_Install(IRQ_GPADC_IDX, (u32)adc_isr, IRQ_ADC_IP) ;
    adc_flag |= B_SADC_ISR_READY;
    local_irq_enable();
    log_info("adc_init\n");

}

void adc_init(void)
{
    adc_isr_init();
    adc_sample_vbg(10);
    return;
}


u32 adc_sample_value(u16 real_ch, u32 re_sap_times)
{
    u32 err = adc_idle(100);
    if (0 != err) {
        return -1;
    }
    u32 ad_value = 0;
    adc_sample(real_ch, 0);
    while (1) {
        while (!(JL_ADC->CON & BIT(ADC_CON_PND)));
        ad_value += JL_ADC->RES;
        if (0 != re_sap_times) {
            re_sap_times--;
        }
        if (0 != re_sap_times) {
            JL_ADC->CON |= BIT(ADC_CON_CPND);
        } else {
            JL_ADC->CON |= BIT(ADC_CON_DONE_PND_CLR);
            break;
        }
    }
    JL_ADC->CON &= ~(BIT(ADC_CON_ADC_EN) | BIT(ADC_CON_ADC_AE) | BIT(ADC_CON_ADC_CLKEN));
    PMU_DET_OE(0);
    PMU_TOADC_EN(0);

    adc_flag &= ~B_SADC_BUSY;
    return ad_value;
}

u16 adc_sample_vbg(u32 re_sap_times)
{
    VBG_BUFFER_EN(1);
    /* udelay(100); */
    VBG_TEST_EN(1);
    /* adc_sample(ADC_CH_PMU_VBG, 0); */
    if (0 == re_sap_times) {
        re_sap_times = 1;
    }
    u32 value = adc_sample_value(ADC_CH_PMU_VBG, re_sap_times);
    u32 res;
    if (-1 != value) {
        vbg_value = value / re_sap_times;
        res = vbg_value;
    } else {
        res = ADC_VALUE_NONE;
    }
    VBG_TEST_EN(0);
    /* udelay(100); */
    VBG_BUFFER_EN(0);//关闭BUFFER

    return res;
}

u32 adc_value2voltage(u32 adc_vbg, u32 adc_ch_val)
{
    if ((0 == adc_vbg) || (ADC_VALUE_NONE == adc_vbg)) {
        return -1;
    }
    u32 adc_res = adc_ch_val;
    u32 vbg_center = 800;
    adc_res = adc_res * vbg_center / adc_vbg;
    return adc_res;
}

u32 adc_get_voltage(u16 real_ch)
{
    u32 adc_res = adc_get_value(real_ch);
    u32 adc_vbg = vbg_value;
    if (adc_vbg == 0) {
        return -1;
    }
    return adc_value2voltage(adc_vbg, adc_res);
}


u32 adc_get_voltage_blocking(u16 real_ch)
{
    adc_sample_vbg(1);
    u32 ad_value = adc_sample_value(real_ch, 1);
    return adc_value2voltage(vbg_value, ad_value);
}

AT_RAM
bool adc_close()
{

    local_irq_disable();
    JL_ADC->CON = 0;
    P3_PMU_ADC0 = 0;
    P3_PMU_ADC1 = 0;


    cur_ch = 0;
    adc_flag &= ~B_SADC_BUSY;

    local_irq_enable();
    return true;
}



