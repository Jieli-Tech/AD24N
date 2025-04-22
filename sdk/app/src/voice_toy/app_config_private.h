#ifndef __APP_CONFIG_PRIVATE_H__
#define __APP_CONFIG_PRIVATE_H__


//*********************************************************************************//
//                        USB Configuration                                        //
//*********************************************************************************//
#if HAS_USB_EN
#define TCFG_USB_EXFLASH_UDISK_ENABLE       DISABLE     //外掛FLASH UDISK
#else
#endif



#if TCFG_PC_ENABLE || TCFG_UDISK_ENABLE
// #include "usb_std_class_def.h"
// #include "usb_common_def.h"

#undef USB_DEVICE_CLASS_CONFIG
#define USB_DEVICE_CLASS_CONFIG             (SPEAKER_CLASS|MIC_CLASS|HID_CLASS)  //配置usb从机模式支持的class
// #define USB_DEVICE_CLASS_CONFIG             (MASSSTORAGE_CLASS|SPEAKER_CLASS|MIC_CLASS|HID_CLASS)  //配置usb从机模式支持的class
#else
#endif

#endif

