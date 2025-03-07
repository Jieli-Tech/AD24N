#include "asm/power_interface.h"
#include "asm/rtc.h"
/* #include "uart.h" */
#include "gpio.h"
#include "board_cfg.h"
/* #include "start/init.h" */

#define LOG_TAG_CONST       PMU
#define LOG_TAG             "[PMU]"
#include "log.h"


extern const char libs_debug;
//-------------------------------------------------------------------
/*config
 */
#ifdef TCFG_UART0_TX_PORT
#define CONFIG_UART_DEBUG_PORT		TCFG_UART0_TX_PORT
#else
#define CONFIG_UART_DEBUG_PORT		-1
#endif

#define DO_PLATFORM_UNINITCALL()			//do_platform_uninitcall()
#define GPIO_CONFIG_UNINIT()				//gpio_config_uninit()

static void usb_high_res()
{
    gpio_set_mode(PORTUSB, 0xffff, PORT_HIGHZ);
}

/*-----------------------------------------------------------------------
 *进入、退出低功耗函数回调状态，函数单核操作、关中断，请勿做耗时操作
 *
 */
static u32 usb_io_con = 0;
void sleep_enter_callback(u8 step)
{
#if 0//KEY_MATRIX_EN
    extern void set_matrixkey_row_port_output();
    // 矩阵按键进低功耗前先把行IO拉低使其可以被唤醒
    set_matrixkey_row_port_output();
#endif
    /* 此函数禁止添加打印 */
    int putchar(int a);
    putchar('<');

    //USB IO打印引脚特殊处理
    if (libs_debug) {
#if ((CONFIG_UART_DEBUG_PORT == IO_PORT_DP) || (CONFIG_UART_DEBUG_PORT == IO_PORT_DM))
        usb_io_con = JL_PORTUSB->DIR;
#endif
    }

    usb_high_res();
}

void sleep_exit_callback(u32 usec)
{
    //USB IO打印引脚特殊处理
    if (libs_debug) {
#if ((CONFIG_UART_DEBUG_PORT == IO_PORT_DP) || (CONFIG_UART_DEBUG_PORT == IO_PORT_DM))
        JL_PORTUSB->DIR = usb_io_con;
#endif
    }

    int putchar(int a);
    putchar('>');

}

static void __mask_io_cfg()
{
    struct app_soft_flag_t app_soft_flag = {0};
    app_soft_flag.sfc_fast_boot = 0;
    mask_softflag_config(&app_soft_flag);
}

u8 power_soff_callback()
{
    DO_PLATFORM_UNINITCALL();

    /* extern_dcdc_switch(0); */

    /* rtc_save_context_to_vm(); */

    __mask_io_cfg();

    gpio_config_soft_poweroff();

    GPIO_CONFIG_UNINIT();

    return 0;
}

void power_early_flowing()
{
    /*默认把低功耗部分代码加载到power overlay段*/
    lowpower_init();

    PORT_TABLE(g);

    init_boot_rom();

    /* printf("get_boot_rom(): %d", get_boot_rom()); */

    //默认关闭MCLR
    /* p33_mclr_sw(0); */

    // 默认关闭长按复位0，由key_driver配置
    /* gpio_longpress_pin0_reset_config(IO_PORTA_00, 0, 8, 1, PORT_INPUT_PULLUP_10K); */
    //长按复位1默认配置8s，写保护
    //gpio_longpress_pin1_reset_config(IO_LDOIN_DET, 1, 8, 1);

    //开打印则开机需要保护打印口，否则初始化完打印后面电源初始化会被设置成高阻
    if (libs_debug) {
        PORT_PROTECT(UART_OUTPUT_CH_PORT);
    }

    power_early_init((u32)gpio_config);
}

int power_later_flowing()
{
    pmu_trim(0, 0);

    power_later_init(0);

    return 0;
}
//late_initcall(power_later_flowing);

struct _p33_io_wakeup_config port0 = {
    .gpio = POWER_WAKEUP_IO,
    .pullup_down_mode = PORT_INPUT_PULLUP_10K,    //输入上拉模式，数字输入、上拉输入、下拉输入
    .filter = PORT_FLT_1ms,
    .edge = FALLING_EDGE,
};

void key_wakeup_init()
{
    p33_io_wakeup_port_init(&port0);
    p33_io_wakeup_enable(POWER_WAKEUP_IO, 1);
}


