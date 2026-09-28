#include "msg.h"
#include "config.h"


#include "audio.h"
#include "audio_dac_cpu.h"
#include "asm/power/power_api.h"
#include "app_config.h"
#include "usb/otg.h"
#include "usb/host/usb_host.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[idle]"
#include "log.h"

void enter_idle_deal(void)
{
    dac_power_off();
#if TCFG_UDISK_ENABLE
    u8 usb_id = 0;
    usb_otg_suspend(usb_id, 0);
    if (usb_otg_online(usb_id) == HOST_MODE) {
        usb_host_unmount(usb_id);
        usb_h_sie_close(usb_id);
        gpio_set_mode(IO_PORT_SPILT(IO_PORT_DP), PORT_INPUT_PULLUP_10K);
        gpio_set_mode(IO_PORT_SPILT(IO_PORT_DM), PORT_INPUT_PULLDOWN_10K);
    }
    usb_otg_suspend(usb_id, OTG_UNINSTALL);
#endif
}

void exit_idle_deal(void)
{
#if TCFG_UDISK_ENABLE
    u8 usb_id = 0;
    usb_otg_resume(usb_id);
#endif
    dac_power_on(SR_DEFAULT, 0);
}

/*----------------------------------------------------------------------------*/
/**@brief   进入power down模式
   @param   usec : -2:静态睡眠，需要等待按键唤醒
                 非-2:睡眠时间，单位us，如1000000即1s后唤醒
   @note    睡眠时间不可超过看门狗唤醒时间的一半
**/
/*----------------------------------------------------------------------------*/
void sys_idle_deal(u32 usec)
{
    enter_idle_deal();
    sys_power_down(usec);
    exit_idle_deal();
}
