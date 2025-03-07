#include "mio_api.h"
#include "vfs.h"
#include "gpio.h"
#include "clock.h"
#include "app_modules.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"
#if defined(HAS_MIO_EN) && (HAS_MIO_EN)

#define MIO_API_PWM_PORT    IO_PORTA_12
#define PWM_FRE             3000

/* MIO最多支持(16 - 已使用PWM通道数)路IO通道 */
#define MIO_API_IO_PORT     JL_PORTA
#define MIO_API_IO_OFFSET   1

u32 mio_a_pwm_cpu_init(u32 chl, u32 gpio, u32 frequency);
u32 mio_a_pwm_cpu_run(u32 chl, u32 pwm_var);
u32 mio_a_pwm_cpu_uninit(u32 chl);
u32 mio_a_io_cpu_init(u32 mask, JL_PORT_TypeDef *port, u32 offset);
u32 mio_a_io_cpu_uninit(u32 mask, JL_PORT_TypeDef *port, u32 offset);

u32 mio_a_read(void *pfile, u8 *buff, u32 len)
{
    return vfs_read(pfile, buff, len);
}

u32 mio_a_pwm_init(u32 chl)
{
    log_info("mio pwm init -> chl : %d\n", chl);
    u32 ret = mio_a_pwm_cpu_init(chl, MIO_API_PWM_PORT, PWM_FRE);
    if (ret) {
        log_error("mio_a_pwm_init ret 0x%x\n", ret);
    }
    return ret;
}

u32 mio_a_pwm_uninit(u32 chl)
{
    u32 ret = mio_a_pwm_cpu_uninit(chl);
    if (ret) {
        log_error("mio_a_pwm_uninit ret 0x%x\n", ret);
    }
    return ret;
}

u32 mio_a_pwm_run(u32 chl, u32 pwm_var)
{
    local_irq_disable();
    u32 ret = mio_a_pwm_cpu_run(chl, pwm_var);
    local_irq_enable();
    if (ret) {
        log_error("mio_a_pwm_run ret 0x%x\n", ret);
    }
    return ret;
}

u32 mio_a_io_init(u32 mask)
{
    u32 ret = mio_a_io_cpu_init(mask, MIO_API_IO_PORT, MIO_API_IO_OFFSET);
    if (ret) {
        log_error("mio_a_io_init ret 0x%x\n", ret);
    }
    return ret;
}

u32 mio_a_io_run(u32 mask, u32 io_ver)
{
    u32 gpio_num = __builtin_ctz(~mask);
    if (gpio_num >= MIO_MAX_CHL_IO) {
        return E_MIO_IO_OUTRANGED;
    }
    MIO_API_IO_PORT->OUT &= ~(mask << MIO_API_IO_OFFSET);
    MIO_API_IO_PORT->OUT |= (io_ver << MIO_API_IO_OFFSET);
    return 0;
}

u32 mio_a_io_uninit(u32 mask)
{
    u32 ret = mio_a_io_cpu_uninit(mask, MIO_API_IO_PORT, MIO_API_IO_OFFSET);
    if (ret) {
        log_error("mio_a_io_uninit ret 0x%x\n", ret);
    }
    return ret;
}

void mio_a_hook_init(sound_mio_obj *obj)
{
    obj->read = mio_a_read;
    obj->pwm_init = mio_a_pwm_init;
    obj->pwm_run = mio_a_pwm_run;
    obj->pwm_uninit = mio_a_pwm_uninit;
    obj->io_init = mio_a_io_init;
    obj->io_run = mio_a_io_run;
    obj->io_uninit = mio_a_io_uninit;
}
#endif
