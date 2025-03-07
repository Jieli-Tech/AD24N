#include "asm/power_interface.h"
#include "board_cfg.h"
#include "audio.h"

#define LOG_TAG_CONST       PMU
#define LOG_TAG             "[PMU]"
#include "log.h"



__attribute__((weak))
void board_gpio_config_soft_poweroff(u32 *gpio_config)
{
    //注册在板卡
}

void gpio_config_soft_poweroff(void)
{
    PORT_TABLE(g);

    log_info("---------------%s--------------", __FUNCTION__);

    PORT_PROTECT(POWER_WAKEUP_IO);
    dac_power_off();
    board_gpio_config_soft_poweroff(gpio_config);
    __port_init((u32)gpio_config);

}

