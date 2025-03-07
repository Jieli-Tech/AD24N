
#include "app_modules.h"
#include "gptimer.h"
#include "gpio_hw.h"
#include "mio_api.h"
#include "errno-base.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[mio_cpu]"
#include "log.h"


#if defined(HAS_MIO_EN) && (HAS_MIO_EN)
u8 MIO_TIMER[MIO_MAX_CHL_PWM];
u32 mio_a_pwm_cpu_init(u32 chl, u32 gpio, u32 frequency)
{
    if (chl >= MIO_MAX_CHL_PWM) {
        return E_MIO_PWM_OUTRANGED;
    }
    u32 gpio_group = gpio / IO_GROUP_NUM;
    u32 gpio_pin = gpio % IO_GROUP_NUM;
    const struct gptimer_config mio_pwm_config = {
        .pwm.freq = frequency, //设置输出频率
        .pwm.duty = 0, //设置占空比为34.56%
        .pwm.port = gpio_group, //设置pwm输出IO_ PORTA_02
        .pwm.pin = BIT(gpio_pin), //设置pwm输出IO_PORTA_02
        .mode = GPTIMER_MODE_PWM, //设置工作模式
    };
    MIO_TIMER[chl] = gptimer_init(TIMER0 + chl, &mio_pwm_config);
    if (MIO_TIMER[chl] == 0xff) {
        return E_MIO_PWM_USING;
    }
    gpio_set_mode(gpio_group, BIT(gpio_pin), PORT_OUTPUT_LOW); //IO口设为输出
    gptimer_start(MIO_TIMER[chl]); //启动timer
    return 0;
}
u32 mio_a_pwm_cpu_run(u32 chl, u32 pwm_var)
{
    if (chl >= MIO_MAX_CHL_PWM) {
        return E_MIO_PWM_OUTRANGED;
    }
    local_irq_disable();
    /* 0 ~ 255 对应 周期 0 ~ 10000 */
    u32 duty = pwm_var * 10000 / 255;
    gptimer_set_pwm_duty(MIO_TIMER[chl], duty);
    local_irq_enable();
    return 0;
}

u32 mio_a_pwm_cpu_uninit(u32 chl)
{
    if (chl >= MIO_MAX_CHL_PWM) {
        return E_MIO_PWM_OUTRANGED;
    }
    gptimer_deinit(MIO_TIMER[chl]);
    return 0;
}

u32 mio_a_io_cpu_init(u32 mask, JL_PORT_TypeDef *port, u32 offset)
{
    u32 gpio_num = __builtin_ctz(~mask);
    if (gpio_num >= MIO_MAX_CHL_IO) {
        return E_MIO_IO_OUTRANGED;
    }
    u32 gpio_group;
    if (port == JL_PORTA) {
        gpio_group = PORTA;
    } else if (port == JL_PORTB) {
        gpio_group = PORTB;
    } else {
        log_error("mio io init this port not support\n");
        return E_MIO_IO_GROUP_OUTRANGED;
    }
    gpio_set_mode(gpio_group, BIT((mask << offset) - 1), PORT_OUTPUT_LOW); //IO口设为输出
    gpio_hw_set_pull_down(gpio_group, BIT((mask << offset) - 1), GPIO_PULLDOWN_10K);
    return 0;
}

u32 mio_a_io_cpu_uninit(u32 mask, JL_PORT_TypeDef *port, u32 offset)
{
    u32 gpio_num = __builtin_ctz(~mask);
    if (gpio_num >= MIO_MAX_CHL_IO) {
        return E_MIO_IO_OUTRANGED;
    }
    u32 gpio_group;
    if (port == JL_PORTA) {
        gpio_group = PORTA;
    } else if (port == JL_PORTB) {
        gpio_group = PORTB;
    } else {
        log_error("mio io uninit this port not support\n");
        return E_MIO_IO_GROUP_OUTRANGED;
    }
    gpio_set_mode(gpio_group, BIT((mask << offset) - 1), PORT_HIGHZ); //IO口设为输出
    return 0;
}

#endif

