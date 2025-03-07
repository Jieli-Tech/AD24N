#include "config.h"
#include "common.h"
#include "app_config.h"
#include "device.h"
#include "norflash.h"
#include "sdmmc/sd_host_api.h"
#include "spi.h"
#include "app_config.h"
#include "usb/otg.h"
#include "board_cfg.h"

#define LOG_TAG_CONST       MAIN
#define LOG_TAG             "[dev list]"
#include "log.h"

// *INDENT-OFF*
//sh58: CLK, DO , DI , d2 , d3
//SPI1: any, any, any, no , no
//SPI2: any, any, any, no , no
#if (EXT_FLASH_EN)
const struct spi_platform_data spix_p_data[HW_SPI_MAX_NUM] = {
    //spi0
    {0},
    //spi1
    {
        .port = {
            EXFLASH_CLK_PORT_SEL, //clk any io
            EXFLASH_D0_PORT_SEL, //do any io
            EXFLASH_D1_PORT_SEL, //di any io
            0xff, //d2 not support
            0xff, //d3 not support
            0xff, //cs any io(主机不操作cs,做从机时才有效)
        },
        .role = SPI_ROLE_MASTER,
        .mode = HW_SPI_WORK_MODE,
        .bit_mode = SPI_FIRST_BIT_MSB,
        .cpol = 0,//clk level in idle state:0:low,  1:high
        .cpha = 0,//sampling edge:0:first,  1:second
        .ie_en = 0, //ie enbale:0:disable,  1:enable
        .irq_priority = 3,
        .spi_isr_callback = NULL,  //spi isr callback
        .clk = 10000000,
    },
#if SUPPORT_SPI2
    //spi2
    {
        .port = {
            IO_PORTA_00, //clk any io
            IO_PORTA_01, //do  any io
            IO_PORTA_02, //di  any io
            0xff,//d2 not support
            0xff,//d3 not support
            0xff,//cs
        },
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE_BIDIR_1BIT,
        .bit_mode = SPI_FIRST_BIT_MSB,
        .cpol = 0,//clk level in idle state:0:low,  1:high
        .cpha = 0,//sampling edge:0:first,  1:second
        .ie_en = 0, //ie enbale:0:disable,  1:enable
        .irq_priority = 3,
        .spi_isr_callback = NULL,  //spi isr callback
        .clk  = 1000000L,
    }
#endif
};
//norflash
NORFLASH_DEV_PLATFORM_DATA_BEGIN(norflash_data)
.spi_hw_num = SPI_HW_NUM,
.spi_cs_port = EXFLASH_CS_PORT_SEL,//any io
.spi_read_width = SPI_READ_DATA_WIDTH,
.spi_pdata = &spix_p_data[SPI_HW_NUM],
.start_addr = 0,
.size = 2 * 1024 * 1024,
NORFLASH_DEV_PLATFORM_DATA_END()
#endif
/************************** otg data****************************/
#if TCFG_OTG_MODE
const struct otg_dev_data otg_data = {
    .usb_dev_en = TCFG_OTG_USB_DEV_EN,
	.slave_online_cnt = TCFG_OTG_SLAVE_ONLINE_CNT,
	.slave_offline_cnt = TCFG_OTG_SLAVE_OFFLINE_CNT,
	.host_online_cnt = TCFG_OTG_HOST_ONLINE_CNT,
	.host_offline_cnt = TCFG_OTG_HOST_OFFLINE_CNT,
	.detect_mode = TCFG_OTG_MODE,
	.detect_time_interval = TCFG_OTG_DET_INTERVAL,
};
#endif


#if TFG_SD_EN
SD0_PLATFORM_DATA_BEGIN(sd0_data)
    .port = {
        SDMMC_CMD_IO,//CMD
        SDMMC_CLK_IO,//CLK
        SDMMC_DAT_IO,//DAT0
    },
    .data_width             = 1,
    .speed                  = 12000000,
#if 0 //CMD检测
    .detect_mode            = SD_CMD_DECT,
    .detect_func            = sdmmc_0_cmd_detect,
#endif
#if 1 //CLK检测
    .detect_mode            = SD_CLK_DECT,
    .detect_func            = sdmmc_0_clk_detect,
    .detect_io_level        = 0,//0:低电平检测到卡  1:高电平检测到卡
#endif
#if 0 //IO检测
    .detect_mode            = SD_IO_DECT,
    .detect_func            = sdmmc_0_io_detect,
    .detect_io              = IO_PORTx_xx,//用于检测的引脚
    .detect_io_level        = x,//0:低电平检测到卡  1:高电平检测到卡
#endif
    .power                  = NULL,
SD0_PLATFORM_DATA_END()

#endif


/************************** otg data****************************/
extern const struct device_operations mass_storage_ops;
REGISTER_DEVICES(device_table) = {
#if (EXT_FLASH_EN)
    {.name = __EXT_FLASH_NANE, .ops = &norfs_dev_ops, .priv_data = (void *) &norflash_data},
#endif
    {.name = __SFC_NANE, .ops = &tzflash_dev_ops, .priv_data = (void *)NULL},
#if TFG_SD_EN
    {.name = __SD0_NANE, .ops = &sd_dev_ops, .priv_data = (void *) &sd0_data},
#endif
#if TCFG_UDISK_ENABLE
    { .name = __UDISK0,     .ops=&mass_storage_ops, .priv_data = (void *)NULL},
#endif
#if TCFG_OTG_MODE
    { .name = __OTG,     .ops=&usb_dev_ops, .priv_data = (void *) &otg_data},
#endif
};

