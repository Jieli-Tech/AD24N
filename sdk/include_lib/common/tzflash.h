#ifndef _TZFLASH_H_
#define _TZFLASH_H_

#include "typedef.h"
#include "boot.h"
// #include "tzspi.h"

//tzspi
#define SPI0_A      0
#define SPI0_B      0x1

#define     SPI_2WIRE_MODE  0
#define     SPI_ODD_MODE    1
#define     SPI_DUAL_MODE   2
#define     SPI_QUAD_MODE   4
#define SPI_AUTO_DATA_WIDTH                 0x100


#define  CACHE_LINE_COUNT  32


#define SPI0_BAUD_FREQ    24000000
#define SPI0_SOURCE_FREQ  clk_get("sys")//br35:spi0 src:hsb

struct tzspi_config_data {
    u8 data_width: 3; //0 1 2 4
    u8 port: 1; //0 1
    u8 baud: 4; //0~15

    u8 rst: 1;// 0 1
    u8 rst_delay_ms: 4; // 0~15
    u8 mode: 3;
};


//tzsfc
#define SFC_SUPPORT_DTR_MODE          1
#define SFC_SUPPORT_PART_NDEC_MODE    1
enum sfcx_mode {
    NORMAL_READ = 0,//old:0b0000  //CMD 1bit, ADDR 1bit clk < 30MHz
    FAST_READ = 1,      //old:0b0001  //CMD 1bit, ADDR 1bit
    DUAL_WIRE_SEND_COMMAND_MODE = 2,//CMD 2 WIRE, ADDR 2 WIRE
    QUAD_WIRE_SEND_COMMAND_MODE = 3,//CMD 4 WIRE, ADDR 4 WIRE
    DUAL_FAST_READ_OUTPUT = 4,      //CMD 1 WIRE, ADDR 1 WIRE  //old:0b0010
    QUAD_FAST_READ_OUTPUT = 5,      //CMD 1 WIRE, ADDR 1 WIRE  //old:0b0011
    DUAL_FAST_READ_IO = 6,          //CMD 1 WIRE, ADDR 2 WIRE  //old:0b0100
    QUAD_FAST_READ_IO = 7,          //CMD 1 WIRE, ADDR 4 WIRE  //old:0b0101

    DUAL_FAST_READ_IO_CONTINUE = 0x16, //CMD 1 WIRE,no cmd ADDR 2 WIRE  //old:0b0110
    QUAD_FAST_READ_IO_CONTINUE = 0x17, //CMD 1 WIRE,no cmd ADDR 4 WIRE//old:0b0111
    QUAD_WIRE_SEND_COMMAND_IO_CONTINUE = 0x13,//CMD 4 WIRE,no cmd ADDR 4 WIRE
};
extern u32 _SFC_DTR_CODE_START;
extern u32 _SFC_DTR_CODE_END;
extern u32 _SFC_MEMORY_START_ADDR[];
extern void sfc_resume(u32 disable_spi);
extern void sfc_suspend(u32 enable_spi);
extern u8 get_sfc_read_mode(void);
extern u8 dec_isd_cfg_ini(const char *cfg, void *value, const u8 *ptr);
extern const u8 sfc0_dtr_mode_en;
extern const s8 sfc0_dtr_dummy_num;
extern const u32 sfc0_dtr_clk_freq;//查看flash文档*2
extern const u8 sfc0_continue_mode_en;
extern void sfc_drop_cache(void *ptr, u32 len);
bool sfc0_is_dtr_mode();
void sfc0_dtr_init(s8 dummy_num);//base:ini
void sfc0_dtr_continue_init(u8 dtr_mode, u8 continue_mode, s8 dummy_num);//base:ini


//tzflash
#define  SFC_FLASH_SPI_READ_MODE_EN 0//1:spi0 read; 0:cpu copy
#define  SFC_FLASH_LOADER2CACHE_MODE_EN    1//loader cache mode

struct tzflash_ndec_cfg_data {
    u32 saddr;
    u32 eaddr;
    struct sfc_info sfc;
};


#define NORFLASH_MUTEX_EN  0


struct flash_reg {
    u8 cmd[4];
    u8 sr_value[4];
    u8 sr_mask[4];
    u8 continue_mode: 4;
    u8 num_of_reg: 2;
    u8 wr_en_mode: 1;
    u8 rev: 1;
};

struct tzflash_operation {
    u32 addr;
    u8 *data_buf;
    u32 data_len;
    u32 cmd_width: 4; //0,1,2,4 //0:表示没有该项
    u32 addr_width: 4; //0,1,2,4

    u32 mode_width: 4; //0,1,2,4
    u32 dummy_width: 4; //0,1,2,4
    u32 dummy_dir: 1; //0:写, 1:读

    u32 write_en_sel: 2; //0:没有，1：non volatile，2：volatile

    u32 data_dir: 1; //0:写, 1:读
    u32 data_width: 4; //0,1,2,4
    u32 wait_ok: 1; //1:wait busy,0:no wait

    u8 cmd;
    u8 mode;
    u8 dummy_len;//byte len
    u8 addr_4byte_en;
};

struct ini_flash_cfg {
    u8 dtr_en;
    u8 continue_en;//2:continue
    u8 wps_en;
};

struct flash_rw {
    u32 addr;
    u32 len;
    void *buffer;
};
int tzflash_cpu2flash_addr(void *logic_addr);
void *tzflash_flash2cpu_addr(u32 phy_addr);
// void tzflash_cfg_dtr_mode();//vm,clock之后



#include "ioctl.h"
#define IOCTL_SET_VM_INFO               _IOW('V', 1, 1)
#define IOCTL_GET_VM_INFO               _IOW('V', 2, 1)

// vm_sfc api
typedef u32(*flash_code_protect_cb_t)(u32 offset, u32 len);
u32 flash_code_protect_callback(u32 offset, u32 len);
// extern volatile u8 vm_busy;
void spi_cache_way_switch(u8 way_num);
// vm擦写时可放出多个中断
void vm_isr_response_list_register(u32 bit_list);
// 兼容旧程序,旧程序vm擦写时只能放出一个中断!
// #define vm_isr_response_index_register(index) vm_isr_response_list_register(BIT(index) | BIT(IRQ_AUDIO_IDX))
void vm_isr_response_index_register(u8 index);
void vm_isr_response_index_unregister(u8 index);
u32 get_vm_isr_response_index_h(void);//获取放出中断的高32位(index 32-63)
u32 get_vm_isr_response_index_l(void);//获取放出中断的低32位(index 0-31)  AD14/15/17只有低32位

#endif

