#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".timer_demo.data.bss")
#pragma data_seg(".timer_demo.data")
#pragma const_seg(".timer_demo.text.const")
#pragma code_seg(".timer_demo.text")
#endif

#define LOG_TAG_CONST       PERI
#define LOG_TAG             "[MCPWM]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE

#include "mcpwm.h"
#include "clock.h"
#include "log.h"

#define printf log_debug
extern void wdt_close();
extern void wdt_clear();

static void mcpwm_detect_callback_0(u32 ch)
{
    log_debug("mcpwm_detect_callback_0\n");
    mdelay(500);
    mcpwm_hw_detect_pnd_clr(ch);
    mcpwm_hw_enable(ch);
}
static void mcpwm_detect_callback_1(u32 ch)
{
    log_debug("mcpwm_detect_callback_1\n");
    mdelay(1000);
    mcpwm_hw_detect_pnd_clr(ch);
    mcpwm_hw_enable(ch);
}

static const struct mcpwm_config cfg_0 = {
    .ch = MCPWM_CH0,
    .mode = MCPWM_EDGE_ALIGNED,
    .freq = 1000,
    .h_duty = 5000,
    .h_pin = IO_PORTA_01,
    .l_duty = 5000,
    .l_pin = IO_PORTA_02,
    .detect_port = IO_PORTA_03,
    .edge = MCPWM_EDGE_FAILL,
    .irq_cb = mcpwm_detect_callback_0,
    .irq_priority = 3,
    .dead_time = 10 * 1000,
};
static const struct mcpwm_config cfg_1 = {
    .ch = MCPWM_CH1,
    .mode = MCPWM_CENTER_ALIGNED,
    .freq = 1 * 1000,
    .h_duty = 5000,
    .h_pin = IO_PORTA_06,
    .l_duty = 5000,
    .l_pin = IO_PORTA_07,
    .detect_port = -1,
    .edge = MCPWM_EDGE_DEFAULT,
    .irq_cb = NULL,
    .irq_priority = 1,
    .dead_time = 0,
};
void mcpwm_demo()
{
    wdt_close();
    log_debug("mcpwm_demo test\n");
    u32 id = mcpwm_init(&cfg_0);
    mcpwm_start(id);
    mdelay(1000);

    mcpwm_pause(id);
    mdelay(1000);

    mcpwm_resume(id);
    mdelay(1000);

    mcpwm_pause(id);
    /* mcpwm_set_freq(id, 1*1000*1000); */
    mcpwm_set_freq(id, 2 * 1000);
    mdelay(1000);
    mcpwm_resume(id);

    mdelay(1000);
    mcpwm_pause(id);
    mcpwm_set_duty(id, 9500, 500);
    mdelay(1000);
    mcpwm_resume(id);

    mcpwm_set_irq_callback(id, 3, mcpwm_detect_callback_1);

    mcpwm_info_dump(id);
    mcpwm_reg_dump(MCPWM_CH0);

    u32 id_1 = mcpwm_init(&cfg_1);
    mcpwm_start(id_1);
    mcpwm_info_dump(id_1);
    mcpwm_reg_dump(MCPWM_CH1);

    while (1) {
        /* mcpwm_deinit(id); */
        /* mdelay(1000); */
        /* id = mcpwm_init(&cfg_0); */
        /* mdelay(1000); */
        wdt_clear();
    }
}
