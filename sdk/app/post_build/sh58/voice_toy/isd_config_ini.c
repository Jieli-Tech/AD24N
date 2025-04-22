#include "app_modules.h"
[TRACE_INFO]
__C_PATH__ = __FILE__;

//#####################################################
//#
//# 配置数据按照 长度+配置名字+数据的方式存储
//#
//#####################################################
[EXTRA_CFG_PARAM]
NEW_FLASH_FS = YES;
CHIP_NAME = AD24N; //8
ENTRY = 0xC000100; //程序入口地址
PID = AD24N; //长度16byte,示例：芯片封装_应用方向_方案名称
VID = 0.01;
RESERVED_OPT = 0;
CHECK_OTA_BIN = NO;

DOWNLOAD_MODEL = SERIAL;
DOWNLOAD_MODEL = usb;
SERIAL_DEVICE_NAME = JlVirtualJtagSerial;
SERIAL_BARD_RATE = 1000000;
SERIAL_CMD_OPT = 2;
SERIAL_CMD_RATE = 100; //[n * 10000]
SERIAL_CMD_RES = 0;
SERIAL_INIT_BAUD_RATE = 9600;
LOADER_BAUD_RATE = 1000000;
LOADER_ASK_BAUD_RATE = 1000000;
BEFORE_LOADER_WAIT_TIME = 150;
SERIAL_SEND_KEY = yes;

//#暂时加上该配置让芯片正常开机，后面问题解决再进行处理
NEED_RESERVED_AREA = NO;

//#是否实时根据FLASH支持的对齐方式组织文件（256字节或4K字节对齐）；
SPECIAL_OPT = 0;
//#强制4k对齐
FORCE_4K_ALIGN = YES;

OTP_CFG_SIZE = CONFIG_VOTP_SIZE;
[BURNER_PASSTHROUGH_CFG]
FLASH_WRITE_PROTECT = YES;

[CHIP_VERSION]
SUPPORTED_LIST = A;

//#####################################################    UBOOT配置项，请勿随意调整顺序    ##################################################
[SYS_CFG_PARAM]
//#data_width[0 1 2 3 4] 3的时候uboot自动识别2或者4线
//#clk [0-255]
//#mode:
//#   0 RD_OUTPUT,       1 cmd       1 addr
//#   1 RD_I/O,          1 cmd       x addr
//#   2 RD_I/O_CONTINUE] no_send_cmd x add
//#port:
//#   0  优先选A端口  CS:PD3  CLK:PD0  D0:PD1  D1:PD2  D2:PB7  D3:PD5
//#   1  优先选B端口  CS:PA13 CLK:PD0  D0:PD1  D1:PA14 D2:PA15 D3:PD5
SPI = 2_3_0_0;
#if (CONFIG_FLASH_DTR_EN)
NORFLASH_DTR_EN = 1;
#endif
#if (CONFIG_FLASH_WPS_EN)
NORFLASH_WPS_EN = 1;
#endif
//#data_clk_mode_port;
//#OSC=btosc;
//#OSC_FREQ=12MHz; #[24MHz 12MHz]
//#SYS_CLK=24MHz;   #[48MHz 24MHz]
UTTX = PA04; //uboot串口tx
UTBD = 1000000; //uboot串口波特率
//#UTRX=PB01;串口升级[PB00 PB05 PA05]
//#RESET=PB01_00_0; //port口_长按时间_有效电平（长按时间有00、01、02、04、08三个值可选，单位为秒，当长按时间为00时，则关闭长按复位功能。）

//# 外部FLASH 硬件连接配置
//#EX_FLASH=PA06_1A_NULL;   //CS_pin / spi (0/1/2) /port(A/B) / power_io
//#EX_FLASH_IO=2_PA10_PA09_PA07_NULL_NULL;  //data_width / CLK_pin / DO_pin / DI_pin / D2_pin / D3_pin   当data_width为4的时候，D2_pin和D3_pin才有效
EX_FLASH = PA05_1A_NULL;    //CS_pin / spi (0/1/2) /port(A/B) / power_io
EX_FLASH_IO = 2_PA11_PA12_PA10_NULL_NULL;   //data_width / CLK_pin / DO_pin / DI_pin / D2_pin / D3_pin   当data_width为4的时候，D2_pin和D3_pin才有效

//#############################################################################################################################################

[FW_ADDITIONAL]
FILE_LIST = (file = ota.bin: type = 100);

//########flash空间使用配置区域###############################################
//#PDCTNAME:    产品名，对应此代码，用于标识产品，升级时可以选择匹配产品名
//#BOOT_FIRST:  1=代码更新后，提示APP是第一次启动；0=代码更新后，不提示
//#UPVR_CTL：   0：不允许高版本升级低版本   1：允许高版本升级低版本
//#XXXX_ADR:    区域起始地址    AUTO：由工具自动分配起始地址
//#XXXX_LEN:    区域长度        CODE_LEN：代码长度
//#XXXX_OPT:    区域操作属性
//#
//#
//#
//#操作符说明  OPT:
//# 0:  下载代码时擦除指定区域
//# 1:  下载代码时不操作指定区域
//# 2:  下载代码时给指定区域加上保护
//############################################################################
[RESERVED_CONFIG]
BTIF_ADR = AUTO;
BTIF_LEN = 0x1000;
BTIF_OPT = 1;

PRCT_ADR = 0;
PRCT_LEN = CODE_LEN;
PRCT_OPT = 2;

VM_ADR = 0;
VM_LEN = 32K;
VM_OPT = 1;

EEPROM_ADR = AUTO;
EEPROM_LEN = 24K;
EEPROM_OPT = 1;



[BURNER_CONFIG]
SIZE = 32;


