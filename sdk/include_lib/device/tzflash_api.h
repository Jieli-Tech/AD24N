#ifndef __TZFLASH_API_H__
#define __TZFLASH_API_H__

#include "typedef.h"
#include "device.h"
#include "tzflash.h"


struct flash_func_len {
    u16 all_len;
    u8 version;
    u8 func_len;//lb
    u8 sta_len;//lb
    u8 func_otp;//lb
    u8 func_dtr;
    u8 func_wp;//bp,tb,sec,cmp
    u8 func_wps;
    u8 func_wps_op;
    u8 func_qe;
    u8 func_srp;
    u8 func_sus;
    u8 func_dc;
    u8 func_drv;
    u8 func_mpm;//dp

    u8 func_qpi;
    u8 func_init;
    u8 func_read_continue;
} __attribute__((packed));
#define TZFLASH_STA_NUM 4
struct flash_sta_cmd {
    u8 sta_cmd_r[TZFLASH_STA_NUM];
    u8 sta_cmd_w[TZFLASH_STA_NUM];
} __attribute__((packed));


struct flash_otp_cfg {//flash otp信息
    u32 otp_offset[5];//otp page的偏移地址组数
    u16 otp_page_size;//otp的page大小
    u8 otp_NumberOfpage;//otp的page数量
    // struct flash_reg lock_cfg[2];//read,write
    u8 wr_en_cmd;//0x50h:sr,0x06h:wr_en
    u8 sr_mask[TZFLASH_STA_NUM];
    u8 sr_value[TZFLASH_STA_NUM];//注意:锁定后不可擦写,sr的值也常是1
} __attribute__((packed));


struct flash_wps_cfg {//flash wl信息
    u8 wr_en_cmd;//0x50h:sr,0x06h:wr_en
    u8 sr_mask[TZFLASH_STA_NUM];
    u8 sr_value[TZFLASH_STA_NUM];//注意:锁定后不可擦写,sr的值也常是1
    // struct flash_reg r_reg_cfg;//read
    // struct flash_reg w_reg_cfg;//write
} __attribute__((packed));

struct flash_wps_op_cfg {
    u8	r_cmd[2];
    u8	w_cmd[2];
    u8	cfg_mask;
    u8	cfg_value;
    struct tzflash_operation read_op;
    struct tzflash_operation write_op;
} __attribute__((packed));

struct flash_wp_cfg {   //写保护配置信息
    u8 numOfwp_array;//写保护参数的个数
    u8 wr_en_cmd;//0x50h:sr,06h:
    u8 sr_mask[TZFLASH_STA_NUM]; //sr1要保留或修改的bit
    struct {
        u8 sr_value[TZFLASH_STA_NUM]; //写保护sr取值
        u16 wp_addr;//写保护结束地址,单位K
    } wp_array[0]; //写保护的组数，修改可变长
} __attribute__((packed));

struct flash_dtr_cfg {
    s8 x1_dummy;//-1:使用默认值
    s8 x2_dummy;//-1:使用默认值
    s8 x4_dummy;//-1:使用默认值
} __attribute__((packed)); //size(1byte)

struct flash_wps_area_cfg {
    u32 start_addr;//任意地址,非对齐时不包含地址所在块
    u32 end_addr;//任意地址,非对齐时不包含地址所在块
    u8 lock_en;//1:保护, 0:解保护
} __attribute__((packed)); //size(1byte)


struct tzspi_target {
    void (*suspend)(u32 id);
    void (*resume)(u32 id);
};
#define REGISTER_TZSPI_TARGET(target) \
        const struct tzspi_target target SEC_USED(.tzspi_target)
extern const struct tzspi_target tzspi_target_begin[];
extern const struct tzspi_target tzspi_target_end[];
#define list_for_each_tzspi_target(p) \
    for (p = tzspi_target_begin; p < tzspi_target_end; p++)



// int tzflash_init(void *arg);
int tzflash_origin_read(struct device *device, u8 *buf, u32 len, u32 offset);
int tzflash_read(struct device *device, void *buffer, u32 len, u32 addr);
int tzflash_write(struct device *device, void *buffer, u32 len, u32 addr);
int tzflash_ioctl(struct device *device, u32 cmd, u32 arg);
void tzflash_dump();
//
// u32 sfc0_flash_addr2cpu_addr(u32 offset);
// void tzflash_read_uuid(u8 *buf);
//
// u32 tzflash_erase_otp();
// u32 tzflash_read_otp(void *buf, u32 len, u32 addr);
// u32 tzflash_write_otp(const u8 *buf, u32 len, u32 addr);
//
// void tzflash_vm_info_set(void *_arg, u8 early_mode);
//
//
// void tzflash_mutex_enter();
// void tzflash_mutex_exit();

u32 syscfg_read_otp(u32 id, u8 *buf, u32 len);
u8 *tzflash_get_uuid(void);
int tzflash_set_write_protect(u8 lock, u32 saddr, u32 eaddr);//return:实际结束地址
void tzflash_change_mode();//dtr(clk),continue,wps
#endif

