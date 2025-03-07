#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__
#include "app_modules.h"

#define ENABLE_THIS_MOUDLE                  1
#define DISABLE_THIS_MOUDLE                 0

#define ENABLE                              1
#define DISABLE                             0

//*********************************************************************************//
//                        KEY VOICE Configuration                                  //
//*********************************************************************************//
#define KEY_VOICE_EN                        0

//*********************************************************************************//
//                        KEY Configuration                                        //
//*********************************************************************************//
#define KEY_IO_EN                           0   ///<IO按键使能
#define KEY_AD_EN                           1   ///<AD按键使能
#define KEY_MATRIX_EN                       0   ///<矩阵按键使能
#define KEY_IR_EN                           0   ///<IR按键使能
#define KEY_TOUCH_EN                        0   ///<触摸按键使能

//*********************************************************************************//
//                        IR_MODE Configuration                                    //
//*********************************************************************************//
#define SIMPLE_IR                           1   //简易红外驱动
#define STANDARD_IR                         2   //标准NEC红外驱动
#define SEL_IR_MODE                         STANDARD_IR

//*********************************************************************************//
//                        Vm Mode Configuration                                    //
//*********************************************************************************//
#define NO_VM                               0
#define USE_NEW_VM                          1
#define USE_OLD_VM                          2
#define SYS_MEMORY_SELECT                   USE_NEW_VM

//*********************************************************************************//
//                        EXFLASH Configuration                                    //
//*********************************************************************************//
#define TCFG_FLASH_SPI_TYPE_SELECT          1//1:flash 选择硬件spi; 0:flash use soft_spi
#define SPI_SD_IO_REUSE                     DISABLE//SPI_FLASH与SD卡模块IO复用使能

#define TFG_SPI_UNIDIR_MODE_EN              DISABLE//外挂flash运行1bit模式
#if TFG_SPI_UNIDIR_MODE_EN
#define HW_SPI_WORK_MODE                    SPI_MODE_UNIDIR_1BIT
#define SOFT_SPI_WORK_MODE                  SPI_MODE_UNIDIR_1BIT//只支持双向或单线
#define SPI_READ_DATA_WIDTH                 1
#else
#define HW_SPI_WORK_MODE                    SPI_MODE_BIDIR_1BIT
#define SOFT_SPI_WORK_MODE                  SPI_MODE_BIDIR_1BIT//只支持双向或单线
#define SPI_READ_DATA_WIDTH                 2
#endif

#if TCFG_FLASH_SPI_TYPE_SELECT
#define SPI_HW_NUM                          1
#else
#define SPI_HW_NUM                          0
#endif

//*********************************************************************************//
//                        SDMMC Configuration                                      //
//*********************************************************************************//
#if defined(TFG_SD_EN) && (TFG_SD_EN)
#define SDMMCA_EN
#endif

//*********************************************************************************//
//                        USB Configuration                                        //
//*********************************************************************************//
#if HAS_USB_EN

#define TCFG_PC_ENABLE                      1//DISABLE  //PC模块使能
#define TCFG_USB_MSD_CDROM_ENABLE           DISABLE
#define TCFG_USB_EXFLASH_UDISK_ENABLE       DISABLE     //外掛FLASH UDISK
#define TCFG_UDISK_ENABLE                   DISABLE     //U盘模块使能
#define TCFG_HID_HOST_ENABLE                DISABLE
#define TCFG_ADB_ENABLE                     DISABLE     //该功能暂不支持
#define TCFG_AOA_ENABLE                     DISABLE     //该功能暂不支持
#define TCFG_PUSH_CODE_ENABLE               DISABLE     //该功能需要关闭OTG使能

#else

#define TCFG_PC_ENABLE                      DISABLE     //PC模块使能
#define TCFG_USB_MSD_CDROM_ENABLE           DISABLE
#define TCFG_USB_EXFLASH_UDISK_ENABLE       DISABLE     //外掛FLASH UDISK
#define TCFG_UDISK_ENABLE                   DISABLE     //U盘模块使能
#define TCFG_HID_HOST_ENABLE                DISABLE
#define TCFG_ADB_ENABLE                     DISABLE     //该功能暂不支持
#define TCFG_AOA_ENABLE                     DISABLE     //该功能暂不支持
#define TCFG_PUSH_CODE_ENABLE               DISABLE     //该功能需要关闭OTG使能
#endif

#define TCFG_USB_PORT_CHARGE                DISABLE     //使能为USB充电模式
#define TCFG_OTG_USB_DEV_EN                 BIT(0)      //USB0 = BIT(0)  USB1 = BIT(1)
#define TCFG_USB_DM_MULTIPLEX_WITH_SD_DAT0  DISABLE     //USBDM与SD_DAT0是否复用


#if TCFG_PC_ENABLE || TCFG_UDISK_ENABLE
#include "usb_std_class_def.h"
#include "usb_common_def.h"

#undef USB_DEVICE_CLASS_CONFIG
#define USB_DEVICE_CLASS_CONFIG             (SPEAKER_CLASS|MIC_CLASS|HID_CLASS)  //配置usb从机模式支持的class

#undef TCFG_OTG_MODE
#define TCFG_OTG_MODE                       (TCFG_OTG_MODE_HOST|TCFG_OTG_MODE_SLAVE|TCFG_OTG_MODE_CHARGE|OTG_DET_DP_ONLY)

#if TCFG_USB_DM_MULTIPLEX_WITH_SD_DAT0
//复用情况下，如果使用此USB口作为充电（即LDO5V_IN连接到此USB口），
//TCFG_OTG_MODE需要或上TCFG_OTG_MODE_CHARGE，用来把charge从host区
//分开；否则不需要，如果LDO5V_IN与其他IO绑定，则不能或上
#define TCFG_DM_MULTIPLEX_WITH_SD_PORT      0//0:sd0  1:sd1 //dm 参与复用的sd配置
#endif

#else

#define USB_DEVICE_CLASS_CONFIG             0
#define TCFG_OTG_MODE                       0

#endif

#if TCFG_PUSH_CODE_ENABLE
// 该功能需要关闭OTG使能
#include "usb_std_class_def.h"
#include "usb_common_def.h"
#undef TCFG_OTG_MODE
#define TCFG_OTG_MODE                       0
#endif

//*********************************************************************************//
//                        PMU Configuration                                        //
//*********************************************************************************//
#define TCFG_LOWPOWER_POWER_SEL             PWR_LDO15                    //电源模式设置，可选DCDC和LDO
#define TCFG_LOWPOWER_BTOSC_DISABLE         0                            //低功耗模式下BTOSC是否保持
#define TCFG_LOWPOWER_LOWPOWER_SEL          1//DEEP_SLEEP_EN                //SNIFF状态下芯片是否进入powerdown
#define TCFG_LOWPOWER_PATTERN               SOFT_MODE//SOFT_BY_POWER_MODE   //选择软关机的方式
#define TCFG_LOWPOWER_VDDIOM_LEVEL          VDDIOM_VOL_30V
#define TCFG_LOWPOWER_VDDIOW_LEVEL          VDDIOW_VOL_28V               //弱VDDIO等级配置
#define TCFG_LOWPOWER_OSC_TYPE              OSC_TYPE_LRC
#define TCFG_LOWPOWER_SOFF                  1
#define TCFG_LOWPOWER_OVERLAY               0

#endif

