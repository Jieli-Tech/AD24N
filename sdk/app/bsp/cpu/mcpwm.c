#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".mcpwm.data.bss")
#pragma data_seg(".mcpwm.data")
#pragma const_seg(".mcpwm.text.const")
#pragma code_seg(".mcpwm.text")
#endif

#include "mcpwm.h"

#define LOG_TAG_CONST       PERI
#define LOG_TAG             "[MCPWM]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE

#include "log.h"

#define MCPWM_MALLOC_ENABLE   0
#if MCPWM_MALLOC_ENABLE
#include "malloc.h"
#else
static struct mcpwm_config _mcpwm_info[MCPWM_NUM_MAX];
#endif

static struct mcpwm_config *mcpwm_info[MCPWM_NUM_MAX] = {NULL};

static u32 mcpwm_malloc(const u32 ch)
{
    u32 id = -1;
    for (u32 _id = 0; _id < MCPWM_NUM_MAX; _id++) {
        if (mcpwm_info[_id] == NULL) {
            id = _id;
            break;
        }
    }
    if (id == -1) {
        log_error("func:%s(),line:%d, mcpwm_info has no idle id\n", __func__, __LINE__);
        return MCPWM_ERR_INIT_FAIL;
    }

#if MCPWM_MALLOC_ENABLE
    mcpwm_info[id] = (struct mcpwm_config *)zalloc(sizeof(struct mcpwm_config));
    assert_d(mcpwm_info[id] != NULL, "func:%s(), line:%d, mcpwm%d malloc fail!\n", __func__, __LINE__, id);
#else
    mcpwm_info[id] = &_mcpwm_info[id];
    memset(mcpwm_info[id], 0, sizeof(struct mcpwm_config));
#endif
    log_info("func:%s(),line:%d, id:%d init success\n", __func__, __LINE__, id);
    return id;
}
static u32 mcpwm_free(const u32 id)
{
#if MCPWM_MALLOC_ENABLE
    free(mcpwm_info[id]);
#else
    mcpwm_info[id] = NULL;
#endif
    return 0;
}

u32 mcpwm_init(const struct mcpwm_config *cfg)
{
    log_debug("func:%s()\n", __func__);
    u32 id = mcpwm_malloc(cfg->ch);
    assert_d(id != MCPWM_ERR_INIT_FAIL, "func:%s(),line:%d, id malloc fail!\n", __func__, __LINE__);
    memcpy(mcpwm_info[id], cfg, sizeof(struct mcpwm_config));

    mcpwm_hw_init(mcpwm_info[id]->ch);
    mcpwm_hw_set_freq_duty(mcpwm_info[id]->ch,
                           mcpwm_info[id]->freq,
                           mcpwm_info[id]->mode,
                           mcpwm_info[id]->h_duty,
                           mcpwm_info[id]->l_duty);
    mcpwm_hw_set_port(mcpwm_info[id]->ch, mcpwm_info[id]->h_pin, mcpwm_info[id]->l_pin);
    mcpwm_hw_set_detect_port(mcpwm_info[id]->ch, mcpwm_info[id]->detect_port, mcpwm_info[id]->edge);
    mcpwm_hw_set_irq_callback(mcpwm_info[id]->ch, mcpwm_info[id]->irq_priority, mcpwm_info[id]->irq_cb);
    mcpwm_hw_set_dead_time(mcpwm_info[id]->ch, mcpwm_info[id]->dead_time);
    return id;
}


u32 mcpwm_deinit(u32 id)
{
    log_debug("func:%s()\n", __func__);
    assert_d(mcpwm_info[id], "%s()\n", __func__);
    mcpwm_hw_deinit(mcpwm_info[id]->ch);
    if (mcpwm_info[id]->h_pin != (u16) - 1) {
        gpio_disable_function(IO_PORT_SPILT(mcpwm_info[id]->h_pin), PORT_FUNC_MCPWM0_H + 2 * mcpwm_info[id]->ch);
        gpio_set_mode(IO_PORT_SPILT(mcpwm_info[id]->h_pin), PORT_HIGHZ);
    }
    if (mcpwm_info[id]->l_pin != (u16) - 1) {
        gpio_disable_function(IO_PORT_SPILT(mcpwm_info[id]->l_pin), PORT_FUNC_MCPWM0_L + 2 * mcpwm_info[id]->ch);
        gpio_set_mode(IO_PORT_SPILT(mcpwm_info[id]->l_pin), PORT_HIGHZ);
    }
    if (mcpwm_info[id]->detect_port != (u16) - 1) {
        gpio_disable_function(IO_PORT_SPILT(mcpwm_info[id]->detect_port), PORT_FUNC_MCPWM0_FP + 2 * mcpwm_info[id]->ch);
        gpio_set_mode(IO_PORT_SPILT(mcpwm_info[id]->detect_port), PORT_HIGHZ);
    }
    mcpwm_free(id);
    return 0;
}

u32 mcpwm_start(u32 id)
{
    log_debug("func:%s()\n", __func__);
    assert_d(mcpwm_info[id], "%s()\n", __func__);
    mcpwm_hw_enable(mcpwm_info[id]->ch);
    return 0;
}
u32 mcpwm_pause(u32 id)
{
    log_debug("func:%s()\n", __func__);
    assert_d(mcpwm_info[id], "%s()\n", __func__);
    mcpwm_hw_disable(mcpwm_info[id]->ch);
    return 0;
}
u32 mcpwm_resume(u32 id)
{
    log_debug("func:%s()\n", __func__);
    assert_d(mcpwm_info[id], "%s()\n", __func__);
    mcpwm_hw_enable(mcpwm_info[id]->ch);
    return 0;
}
u32 mcpwm_set_freq(u32 id, u32 freq)
{
    log_debug("func:%s()\n", __func__);
    assert_d(mcpwm_info[id], "%s()\n", __func__);
    mcpwm_info[id]->freq = freq;
    mcpwm_hw_set_freq_duty(mcpwm_info[id]->ch,
                           mcpwm_info[id]->freq,
                           mcpwm_info[id]->mode,
                           mcpwm_info[id]->h_duty,
                           mcpwm_info[id]->l_duty);
    return 0;
}
u32 mcpwm_set_duty(u32 id, u32 h_duty, u32 l_duty)
{
    log_debug("func:%s()\n", __func__);
    assert_d(mcpwm_info[id], "%s()\n", __func__);
    mcpwm_info[id]->h_duty = h_duty;
    mcpwm_info[id]->l_duty = l_duty;
    mcpwm_hw_set_freq_duty(mcpwm_info[id]->ch,
                           mcpwm_info[id]->freq,
                           mcpwm_info[id]->mode,
                           mcpwm_info[id]->h_duty,
                           mcpwm_info[id]->l_duty);
    return 0;
}
u32 mcpwm_set_irq_callback(u32 id, u32 priority, mcpwm_detect_irq_cb irq_cb)
{
    log_debug("func:%s()\n", __func__);
    assert_d(mcpwm_info[id], "%s()\n", __func__);
    mcpwm_info[id]->irq_priority = priority;
    mcpwm_info[id]->irq_cb = irq_cb;
    mcpwm_hw_set_irq_callback(mcpwm_info[id]->ch, mcpwm_info[id]->irq_priority, mcpwm_info[id]->irq_cb);
    return 0;
}

u32 mcpwm_info_dump(u32 id)
{
    log_debug("mcpwm_info[%d]->ch = %d\n", id, (u32)mcpwm_info[id]->ch);
    log_debug("mcpwm_info[%d]->mode = %d\n", id, (u32)mcpwm_info[id]->mode);
    log_debug("mcpwm_info[%d]->freq = %d\n", id, (u32)mcpwm_info[id]->freq);
    log_debug("mcpwm_info[%d]->h_duty = %d\n", id, (u32)mcpwm_info[id]->h_duty);
    log_debug("mcpwm_info[%d]->h_pin = %d\n", id, (u32)mcpwm_info[id]->h_pin);
    log_debug("mcpwm_info[%d]->l_duty = %d\n", id, (u32)mcpwm_info[id]->l_duty);
    log_debug("mcpwm_info[%d]->l_pin = %d\n", id, (u32)mcpwm_info[id]->l_pin);
    log_debug("mcpwm_info[%d]->detect_port = %d\n", id, (u32)mcpwm_info[id]->detect_port);
    log_debug("mcpwm_info[%d]->edge = %d\n", id, (u32)mcpwm_info[id]->edge);
    log_debug("mcpwm_info[%d]->irq_cb = %d\n", id, (u32)mcpwm_info[id]->irq_cb);
    log_debug("mcpwm_info[%d]->irq_priority = %d\n", id, (u32)mcpwm_info[id]->irq_priority);
    return 0;
}



















