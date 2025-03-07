#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".mcpwm.data.bss")
#pragma data_seg(".mcpwm.data")
#pragma const_seg(".mcpwm.text.const")
#pragma code_seg(".mcpwm.text")
#endif

#include "clock.h"
#include "mcpwm.h"

#define LOG_TAG_CONST       PERI
#define LOG_TAG             "[MCPWM]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE

#include "log.h"

#define MCPWM_SPINLOCK_EN 0
#if (MCPWM_SPINLOCK_EN && (CPU_CORE_NUM > 1))
/* #include "spinlock.h" */
/* static spinlock_t timer_lock; */
#define MCPWM_ENTER_CRITICAL() \
    unmask_enter_critical()

#define MCPWM_EXIT_CRITICAL() \
    unmask_exit_critical()
#else
#define MCPWM_ENTER_CRITICAL()
#define MCPWM_EXIT_CRITICAL()
#endif
#define MCPWM_CLK   clk_get("mcpwm")


static MCPWM_TIMERx_REG *mcpwm_get_timerx_reg(mcpwm_ch ch)
{
    assert_d((u32)ch < MCPWM_CH_MAX, "func:%s(), line:%d\n", __func__, __LINE__);
    MCPWM_TIMERx_REG *reg = (MCPWM_TIMERx_REG *)(MCPWM_TMR_BASE_ADDR + ch * MCPWM_TMR_OFFSET);
    return reg;
}
static MCPWM_CHx_REG *mcpwm_get_chx_reg(mcpwm_ch ch)
{
    assert_d((u32)ch < MCPWM_CH_MAX, "func:%s(), line:%d\n", __func__, __LINE__);
    MCPWM_CHx_REG *reg = (MCPWM_CHx_REG *)(MCPWM_CH_BASE_ADDR + ch * MCPWM_CH_OFFSET);
    return reg;
}
static MCPWM_FPINx_REG *mcpwm_get_fpinx_reg(mcpwm_ch ch)
{
    assert_d((u32)ch < MCPWM_CH_MAX, "func:%s(), line:%d\n", __func__, __LINE__);
    MCPWM_FPINx_REG *reg = (MCPWM_FPINx_REG *)(MCPWM_FPIN_BASE_ADDR + ch * MCPWM_FPIN_OFFSET);
    return reg;
}
static MCPWM_CONx_REG *mcpwm_get_conx_reg(mcpwm_ch ch)
{
    assert_d((u32)ch < MCPWM_CH_MAX, "func:%s(), line:%d\n", __func__, __LINE__);
    MCPWM_CONx_REG *reg = (MCPWM_CONx_REG *)(MCPWM_CON_BASE_ADDR + ch * MCPWM_CON_OFFSET);
    return reg;
}

static void mcpwm_hw_clk_en_check(mcpwm_ch ch)
{
    MCPWM_ENTER_CRITICAL();
    MCPWM_CONx_REG *mcpwm_con_reg = mcpwm_get_conx_reg(ch);
    if ((mcpwm_con_reg->con0 & BIT(MCPWM_CON_CLK_EN)) == 0) {
        mcpwm_con_reg->con0 |= BIT(MCPWM_CON_CLK_EN);
    }
    MCPWM_EXIT_CRITICAL();
}
static void mcpwm_hw_reg_clear(mcpwm_ch ch)
{
    MCPWM_ENTER_CRITICAL();
    MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);
    timer_reg->con = BIT(MCPWM_TMR_UFCLR) | BIT(MCPWM_TMR_OFCLR);
    timer_reg->prd = 0;
    timer_reg->cnt = 0;

    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
    ch_reg->con0 = 0;
    ch_reg->con1 = 0;
    ch_reg->cmph = 0;
    ch_reg->cmpl = 0;

    MCPWM_FPINx_REG *fpin_reg = mcpwm_get_fpinx_reg(ch);
    fpin_reg->con0 &= ~BIT(MCPWM_FPIN_FLT_EN + ch);

    MCPWM_CONx_REG *mcpwm_con_reg = mcpwm_get_conx_reg(ch);
    mcpwm_con_reg->con0 &= ~BIT(ch + MCPWM_CON_PWM_EN);
    mcpwm_con_reg->con0 &= ~BIT(ch + MCPWM_CON_TMR_EN);
    MCPWM_EXIT_CRITICAL();
}

static u32 mcpwm_hw_set_clk_src_div(u32 clk_src, u32 *clk_div, u32 freq)
{
    u32 ret = 1;
    u32 freq_min;
    for (u8 i = 0; i < BIT(MCPWM_TMR_CKPS_); i++) {
        freq_min = clk_src / (MCPWM_TMR_PRD_SIZE << i);
        if (freq >= freq_min) {
            *clk_div = i;
            ret = 0;
            break;
        }
    }
    return ret;
}

u32 mcpwm_hw_init(u32 ch)
{
    mcpwm_hw_reg_clear(ch);
    mcpwm_hw_clk_en_check(ch);

    return 0;
}
u32 mcpwm_hw_deinit(u32 ch)
{
    mcpwm_hw_disable(ch);
    mcpwm_hw_reg_clear(ch);
    return 0;
}
u32 mcpwm_hw_pause(u32 ch)
{
    return mcpwm_hw_disable(ch);
}
u32 mcpwm_hw_resume(u32 ch, u32 con)
{
    MCPWM_CONx_REG *mcpwm_con_reg = mcpwm_get_conx_reg(ch);
    MCPWM_ENTER_CRITICAL();
    mcpwm_con_reg->con0 = con;
    MCPWM_EXIT_CRITICAL();
    return 0;
}
u32 mcpwm_hw_enable(u32 ch)
{
    MCPWM_CONx_REG *mcpwm_con_reg = mcpwm_get_conx_reg(ch);
    MCPWM_ENTER_CRITICAL();
    mcpwm_con_reg->con0 |= BIT(ch + MCPWM_CON_TMR_EN);
    mcpwm_con_reg->con0 |= BIT(ch + MCPWM_CON_PWM_EN);
    MCPWM_EXIT_CRITICAL();
    return 0;
}
u32 mcpwm_hw_disable(u32 ch)
{
    MCPWM_CONx_REG *mcpwm_con_reg = mcpwm_get_conx_reg(ch);
    u32 mcpwm_con = mcpwm_con_reg->con0;
    MCPWM_ENTER_CRITICAL();
    mcpwm_con_reg->con0 &= ~BIT(ch + MCPWM_CON_PWM_EN);
    mcpwm_con_reg->con0 &= ~BIT(ch + MCPWM_CON_TMR_EN);
    MCPWM_EXIT_CRITICAL();
    return mcpwm_con;
}
u32 mcpwm_hw_set_freq_duty(u32 ch, u32 freq, mcpwm_aligned_mode mode, u32 h_duty, u32 l_duty)
{
    u32 mcpwm_con = mcpwm_hw_pause(ch);

    u32 clk_src = MCPWM_CLK;
    u32 clk_div;
    if (mcpwm_hw_set_clk_src_div(clk_src, &clk_div, freq)) {
        log_error("func:%s(),line:%d, clk_div error\n", __func__, __LINE__);
        return MCPWM_ERR_INIT_FAIL;
    }
    log_debug("clk_src:%d, clk_div:%d\n", clk_src, clk_div);
    u32 tmr_con = 0;
    u32 tmr_cnt = 0;
    u32 tmr_prd = 0;
    SFR(tmr_con, MCPWM_TMR_CKPS, MCPWM_TMR_CKPS_, clk_div);
    if (mode == MCPWM_EDGE_ALIGNED) {
        SFR(tmr_con, MCPWM_TMR_MODE, MCPWM_TMR_MODE_, 0b01);
        /* tmr_prd = (clk_src / BIT(clk_div)) / freq - 1; */
        tmr_prd = (2 * clk_src + freq * BIT(clk_div)) / (2 * freq * BIT(clk_div)) - 1; //四舍五入
    } else {
        SFR(tmr_con, MCPWM_TMR_MODE, MCPWM_TMR_MODE_, 0b10);
        /* tmr_prd = (clk_src / BIT(clk_div)) / (freq * 2) - 1; */
        tmr_prd = (clk_src + freq * BIT(clk_div)) / (2 * freq * BIT(clk_div)) - 1; //四舍五入
    }
    u32 ch_cmph = tmr_prd * h_duty / 10000;
    u32 ch_cmpl = tmr_prd * l_duty / 10000;
    MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);
    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
    MCPWM_ENTER_CRITICAL();
    timer_reg->con = tmr_con;
    timer_reg->cnt = tmr_cnt;
    timer_reg->prd = tmr_prd;
    ch_reg->cmph = ch_cmph;
    ch_reg->cmpl = ch_cmpl;
    MCPWM_EXIT_CRITICAL();

    mcpwm_hw_resume(ch, mcpwm_con);
    return 0;
}
u32 mcpwm_hw_set_port(u32 ch, u32 h_pin, u32 l_pin)
{
    u32 mcpwm_con = mcpwm_hw_pause(ch);

    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
    u32 ch_con0 = ch_reg->con0;
    u32 ch_con1 = ch_reg->con1;
    if (h_pin < IO_MAX_NUM) {
        ch_con0 |= BIT(MCPWM_CH_H_EN);
        gpio_set_mode(IO_PORT_SPILT(h_pin), PORT_OUTPUT_LOW);
        gpio_set_function(IO_PORT_SPILT(h_pin), PORT_FUNC_MCPWM0_H + 2 * ch);
    } else {
        ch_con0 &= ~BIT(MCPWM_CH_H_EN);
    }
    if (l_pin < IO_MAX_NUM) {
        ch_con0 |= BIT(MCPWM_CH_L_EN);
        gpio_set_mode(IO_PORT_SPILT(l_pin), PORT_OUTPUT_LOW);
        gpio_set_function(IO_PORT_SPILT(l_pin), PORT_FUNC_MCPWM0_L + 2 * ch);
    } else {
        ch_con0 &= ~BIT(MCPWM_CH_L_EN);
    }
    SFR(ch_con1, MCPWM_CH_TMRSEL, MCPWM_CH_TMRSEL_, ch);
    MCPWM_ENTER_CRITICAL();
    ch_reg->con0 = ch_con0;
    ch_reg->con1 = ch_con1;
    MCPWM_EXIT_CRITICAL();

    mcpwm_hw_resume(ch, mcpwm_con);
    return 0;
}
u32 mcpwm_hw_set_detect_port(u32 ch, u32 port, mcpwm_edge edge)
{
    u32 mcpwm_con = mcpwm_hw_pause(ch);

    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
    u32 ch_con1 = ch_reg->con1;
    MCPWM_FPINx_REG *fpin_reg = mcpwm_get_fpinx_reg(ch);
    u32 fpin_con = fpin_reg->con0;
    if (port != (u16) - 1) {
        ch_con1 |= BIT(MCPWM_CH_FPINEN) | BIT(MCPWM_CH_FPINAUTO);
        SFR(ch_con1, MCPWM_CH_FPINSEL, MCPWM_CH_FPINSEL_, ch);
        if (edge == MCPWM_EDGE_RISE) {
            gpio_set_mode(IO_PORT_SPILT(port), PORT_INPUT_PULLDOWN_10K);
            fpin_con |= BIT(MCPWM_FPIN_EDGE + ch);
        } else if (edge == MCPWM_EDGE_FAILL) {
            gpio_set_mode(IO_PORT_SPILT(port), PORT_INPUT_PULLUP_10K);
            fpin_con &= ~BIT(MCPWM_FPIN_EDGE + ch);
        }
        fpin_con |=  BIT(MCPWM_FPIN_FLT_EN + ch);//开启滤波
        fpin_con |= (0b111111 << MCPWM_FPIN_FLT_PR); //滤波时间 = 16 * 64 / lsb_clk (单位：s)
        gpio_set_function(IO_PORT_SPILT(port), PORT_FUNC_MCPWM0_FP + ch);
    }
    MCPWM_ENTER_CRITICAL();
    ch_reg->con1 = ch_con1;
    fpin_reg->con0 = fpin_con;
    MCPWM_EXIT_CRITICAL();

    mcpwm_hw_resume(ch, mcpwm_con);
    return 0;
}
static void (*mcpwm_cb_table[MCPWM_CH_MAX])(u32 ch);
static void mcpwm_cb(u32 ch)
{
    if (mcpwm_cb_table[ch]) {
        mcpwm_cb_table[ch](ch);
    }
    asm("csync");
}
___interrupt
static void mcpwm_fpin_cb()
{
    MCPWM_CHx_REG *ch_reg = NULL;
    for (u8 ch = 0; ch < MCPWM_CH_MAX; ch++) {
        ch_reg = mcpwm_get_chx_reg(ch);
        if (ch_reg->con1 & BIT(MCPWM_CH_FPND)) {
            if (ch_reg->con1 & BIT(MCPWM_CH_INTEN)) {
                ch_reg->con1 &= ~BIT(MCPWM_CH_INTEN); //关闭故障保护输入IE使能
                mcpwm_cb(ch);
            }
        }
    }
}
u32 mcpwm_hw_set_irq_callback(u32 ch, u32 priority, mcpwm_detect_irq_cb irq_cb)
{
    u32 mcpwm_con = mcpwm_hw_pause(ch);

    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
    u32 ch_con1 = ch_reg->con1;
    if (irq_cb) {
        ch_con1 |= BIT(MCPWM_CH_FCLR) | BIT(MCPWM_CH_INTEN);
        mcpwm_cb_table[ch] = irq_cb;
        request_irq(IRQ_MCPWM_CHX_IDX, priority, mcpwm_fpin_cb, 0);
    } else {
        ch_con1 |= BIT(MCPWM_CH_FCLR);
        ch_con1 &= ~BIT(MCPWM_CH_INTEN);
        mcpwm_cb_table[ch] = NULL;
    }
    MCPWM_ENTER_CRITICAL();
    ch_reg->con1 = ch_con1;
    MCPWM_EXIT_CRITICAL();

    mcpwm_hw_resume(ch, mcpwm_con);
    return 0;
}
u32 mcpwm_hw_set_dead_time(u32 ch, u32 ns)
{
    u32 mcpwm_con = mcpwm_hw_pause(ch);

    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
    u32 ch_con0 = ch_reg->con0;

    if (ns == 0) {
        ch_con0 &= ~BIT(MCPWM_CH_DTEN);
    } else {
        u32 clk_src = MCPWM_CLK;
        u8 dtckps;
        u8 dtpr;
        for (dtckps = 0; dtckps <= 0xf; dtckps++) {
            if ((1000000000 / (clk_src >> dtckps) * 32) > ns) {
                dtpr = ns / (1000000000 / (clk_src >> dtckps)) - 1;
                SFR(ch_con0, MCPWM_CH_DTCKPS, MCPWM_CH_DTCKPS_, dtckps);
                SFR(ch_con0, MCPWM_CH_DTPR, MCPWM_CH_DTPR_, dtpr);
                ch_con0 |= BIT(MCPWM_CH_DTEN);
                break;
            }
        }
    }

    MCPWM_ENTER_CRITICAL();
    ch_reg->con0 = ch_con0;
    MCPWM_EXIT_CRITICAL();

    mcpwm_hw_resume(ch, mcpwm_con);
    return 0;
}
u32 mcpwm_hw_detect_pnd_clr(u32 ch)
{
    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
    u32 ch_con1 = ch_reg->con1;
    ch_con1 |= BIT(MCPWM_CH_FCLR) | BIT(MCPWM_CH_INTEN);
    MCPWM_ENTER_CRITICAL();
    ch_reg->con1 = ch_con1;
    MCPWM_EXIT_CRITICAL();
    return 0;
}
u32 mcpwm_reg_dump(u32 ch)
{
    MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);
    log_debug("TMR%d_CON 0x%x\n", ch, (u32)timer_reg->con);
    log_debug("TMR%d_CNT 0x%x\n", ch, (u32)timer_reg->cnt);
    log_debug("TMR%d_PRD 0x%x\n", ch, (u32)timer_reg->prd);
    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
    log_debug("CH%d_CON0 0x%x\n", ch, (u32)ch_reg->con0);
    log_debug("CH%d_CON1 0x%x\n", ch, (u32)ch_reg->con1);
    log_debug("CH%d_CMPH 0x%x\n", ch, (u32)ch_reg->cmph);
    log_debug("CH%d_CMPL 0x%x\n", ch, (u32)ch_reg->cmpl);
    MCPWM_FPINx_REG *fpin_reg = mcpwm_get_fpinx_reg(ch);
    log_debug("FPIN_CON 0x%x\n", (u32)fpin_reg->con0);
    MCPWM_CONx_REG *mcpwm_con_reg = mcpwm_get_conx_reg(ch);
    log_debug("MCPWM_CON 0x%x\n", (u32)mcpwm_con_reg->con0);

    return 0;
}

#include "asm/power_interface.h"
/* #include "power/include/power_flow.h" */
#include "app_config.h"
#if (TCFG_LOWPOWER_LOWPOWER_SEL == DEEP_SLEEP_EN)

static MCPWM_TIMERx_REG _timer_reg[MCPWM_CH_MAX];
static MCPWM_CHx_REG _ch_reg[MCPWM_CH_MAX];
static MCPWM_FPINx_REG _fpin_reg;
static MCPWM_CONx_REG _mcpwm_con_reg;
static u8 mcpwm_enter_deepsleep(void)
{
    MCPWM_CONx_REG *mcpwm_con_reg = mcpwm_get_conx_reg(0);
    _mcpwm_con_reg.con0 = mcpwm_con_reg->con0;
    MCPWM_FPINx_REG *fpin_reg = mcpwm_get_fpinx_reg(0);
    _fpin_reg.con0 = fpin_reg->con0;
    for (u32 ch = 0; ch < MCPWM_CH_MAX; ch++) {
        MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);
        _timer_reg[ch].con = timer_reg->con;
        _timer_reg[ch].cnt = timer_reg->cnt;
        _timer_reg[ch].prd = timer_reg->prd;
        MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
        _ch_reg[ch].con0 = ch_reg->con0;
        _ch_reg[ch].con1 = ch_reg->con1;
        _ch_reg[ch].cmph = ch_reg->cmph;
        _ch_reg[ch].cmpl = ch_reg->cmpl;
    }

    return 0;
}

static u8 mcpwm_exit_deepsleep(void)
{
    for (u32 ch = 0; ch < MCPWM_CH_MAX; ch++) {
        MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);
        timer_reg->prd = _timer_reg[ch].prd;
        timer_reg->cnt = _timer_reg[ch].cnt;
        timer_reg->con = _timer_reg[ch].con;

        MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
        ch_reg->con0 = _ch_reg[ch].con0;
        ch_reg->con1 = _ch_reg[ch].con1 ;
        ch_reg->cmph = _ch_reg[ch].cmph;
        ch_reg->cmpl = _ch_reg[ch].cmpl;
    }
    MCPWM_FPINx_REG *fpin_reg = mcpwm_get_fpinx_reg(0);
    fpin_reg->con0 = _fpin_reg.con0;
    MCPWM_CONx_REG *mcpwm_con_reg = mcpwm_get_conx_reg(0);
    mcpwm_con_reg->con0 = _mcpwm_con_reg.con0;

    return 0;
}
DEEPSLEEP_TARGET_REGISTER(gptimer) = {
    .name   = "mcpwm",
    .enter  = mcpwm_enter_deepsleep,
    .exit   = mcpwm_exit_deepsleep,
};
#endif
