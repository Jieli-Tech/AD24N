#include "asm/power_interface.h"
#include "vm_api.h"
#include "app_config.h"


void sys_power_down(u32 usec)
{
    if (usec == -2) {
        wdt_close();
    }
#if TCFG_LOWPOWER_OVERLAY
    lowpower_init();
#endif
    vPortSuppressTicksAndSleep(usec);
#if TCFG_LOWPOWER_OVERLAY
    lowpower_uninit();
#endif

    wdt_init(WDT_8S);
}
void sys_softoff(void)
{
    vm_pre_erase();
#if TCFG_LOWPOWER_OVERLAY
    lowpower_init();
#endif
    power_set_soft_poweroff();
#if TCFG_LOWPOWER_OVERLAY
    lowpower_uninit();
#endif
}
