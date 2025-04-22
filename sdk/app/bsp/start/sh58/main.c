/*********************************************************************************************
    *   Filename        : main.c

    *   Description     :

    *   Author          :

    *   Email           :

    *   Last modifiled  :

    *   Copyright:(c)JIELI  2011-2017  @ , All Rights Reserved.
*********************************************************************************************/
#include "config.h"
#include "common.h"
#include "maskrom.h"
#include "app_config.h"
#include "init.h"
#include "efuse.h"
#include "asm/power_interface.h"
#include "clock.h"
/* #include "vm_api.h" */
/* #include "power_api.h" */

#define LOG_TAG_CONST       MAIN
#define LOG_TAG             "[main]"
#include "log.h"



extern void enter_critical_hook_init(void *enter);
extern void exit_critical_hook_init(void *enter);
void critical_hook_init()
{
    enter_critical_hook_init((void *)enter_critical_hook);
    exit_critical_hook_init((void *)exit_critical_hook);
}

void maskrom_init(void)
{
    /* log_info("maskrom_init\n"); */
    struct maskrom_argv argv;
    memset((void *)&argv, 0, sizeof(struct maskrom_argv));
    argv.pchar = (void *)putchar; //sh58 maskrom 没有包含print驱动
    argv.exp_hook = exception_analyze;
    argv.local_irq_enable = NULL;
    argv.local_irq_disable = NULL;
    argv.flt = NULL;

    extern void exception_irq_handler(void);
    request_irq(1, 7, (void *)exception_irq_handler, 0);
    mask_init(&argv);
    emu_init();
}
extern void app(void);
extern void emu_init(void);
extern void exception_analyze(unsigned int *sp);

/*----------------------------------------------------------------------------*/
/**@brief   电源以及时钟初始化，此函数严禁修改！！！！！！！
   @author  liujie
   @note    void immutable_initialize_clock_power()
*/
/*----------------------------------------------------------------------------*/
void immutable_initialize_clock_power()
{
    //此函数涉及电源和时钟的初始化，有严格的顺序要求，严禁对此函数的修改。
    efuse_init();
    critical_hook_init();
    early_system_init();
    clock_set_sfc_max_freq(SPI_MAX_CLK);
    clk_voltage_init(CLOCK_MODE_ADAPTIVE, DVDD_VOL_123V);

    //----clk_early_init函数运行期间禁止打印，否则有死机风险！！！！！！！！
    //----clk_early_init运行启动会停时钟，所有有参考时钟的外设全部会受影响，影响不限于程序卡死及外设运行失常。
    register_handle_printf_putchar(NULL);
    clk_early_init(PLL_REF_LRC, 200000, PLL_MAX_LIMIT);
    register_handle_printf_putchar(putchar);

    power_early_flowing();

    //----sys_pll_ldo_trim_check函数运行期间禁止打印，否则有死机风险！！！！！！！！
    //----sys_pll_ldo_trim_check运行启动会停时钟，所有有参考时钟的外设全部会受影响，影响不限于程序卡死及外设运行失常。
    sys_pll_ldo_trim_check();//该函数会短暂关闭pll,std
    board_power_init();
    clock_dump();
    efuse_dump();
    power_later_flowing();
}

int c_main(int cfg_addr)
{
    maskrom_init();
    wdt_init(WDT_8S);

    register_handle_printf_putchar(putchar);
    log_init(1000000);
    log_info("--------sh58 apps-------------\n");
    immutable_initialize_clock_power();
    log_info("hello world\n");

    /* log_info("flash_size 0x%x\n", boot_info.flash_size); */
    /* log_info("chip_id 0x%x\n", boot_info.chip_id); */
    /* log_info("trim_value 0x%x\n", boot_info.flash_size); */
    /* log_info("up_suc_flag 0x%x\n", boot_info.up_suc_flag); */

    /* extern void audio_dac_test_demo(void); */
    /* audio_dac_test_demo(); */

    /* extern void audio_adc_test_demo(void); */
    /* audio_adc_test_demo(); */

    system_init();

    app();
    while (1) {
        wdt_clear();
    }
    return 0;

}

void cpu_assert_debug(void)
{
    /* 用户自行决定断言操作 */
    /* 默认不操作 */
}

