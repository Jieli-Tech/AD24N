#include "config.h"
#include "common.h"
#include "tick_timer_driver.h"
#include "boot.h"
#include "msg.h"
#include "audio.h"
#include "dac.h"
#include "dac_api.h"
/* #include "audio_adc.h" */
#include "vfs.h"
/* #include "midi_api.h" */
#include "src_api.h"
#include "device.h"
#include "ioctl_cmds.h"
#include "vm_api.h"
#include "nor_fs/nor_fs.h"
#include "key.h"
#include "init.h"
#include "adc_drv.h"
#include "clock.h"
#include "app_power_mg.h"
/* #include "efuse.h" */
#include "my_malloc.h"
#include "mio_api.h"
#include "sine_play.h"
/* #include "list/midi_ctrl_api.h" */
/* #include "efuse_trim_value.h" */
/* #include "vm_sfc.h" */
#include "tzflash.h"
#include "tzflash_api.h"
#include "code_v2/update.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"



extern void exception_irq_handler(void);
void all_init_isr(void)
{
    u32 i;
    unsigned int *israddr = (unsigned int *)IRQ_MEM_ADDR;
    for (i = 0; i < 32; i++) {
        israddr[i] = (u32)exception_irq_handler;
    }
}

struct sydf_cfg_info {
    u32 votp_cfg_addr;
    u32 votp_cfg_len;
    u32 ini_cfg_addr;
    u32 ini_cfg_len;
};
struct sydf_cfg_info sydf_cfg;


void sydfile_get_votp_cfg_addr_len(u32 *votp_cfg_addr, u32 *votp_cfg_len)
{
    *votp_cfg_addr = sydf_cfg.votp_cfg_addr;
    *votp_cfg_len = sydf_cfg.votp_cfg_len;
    /* y_printf(">>>[test]:votp addr = 0x%x, len = 0x%x\n", sydf_cfg.votp_cfg_addr, sydf_cfg.votp_cfg_len); */
}
void sydfile_get_ini_cfg_info(u32 *ini_cfg_addr, u32 *ini_cfg_len)
{
    *ini_cfg_addr = sydf_cfg.ini_cfg_addr;
    *ini_cfg_len = sydf_cfg.ini_cfg_len;
}

static struct vfs_attr vm_attr;
int flash_info_init(void)
{
    u32 err;
    void *pvfs = 0;
    void *pvfile = 0;
    void *device = 0;
    u32 capacity = 0;

    err = vfs_mount(&pvfs, (void *)NULL, (void *) NULL);
    ASSERT(!err, "fii vfs mount : 0x%x\n", err)
    err = vfs_openbypath(pvfs, &pvfile, "/app_area_head/VM");
    ASSERT(!err, "fii vfs openbypath : 0x%x\n", err)
    err = vfs_ioctl(pvfile, FS_IOCTL_FILE_ATTR, (int)&vm_attr);
    ASSERT(!err, "fii vfs ioctl : 0x%x\n", err)
    log_info("file size : 0x%x\nfile sclust : 0x%x\n", vm_attr.fsize, vm_attr.sclust);
    /* log_info("boot info 0x%x\n", boot_info.flash_size); */
    vfs_file_close(&pvfile);
    vfs_fs_close(&pvfs);

    boot_info.vm.vm_saddr = vm_attr.sclust;
    boot_info.vm.vm_size = vm_attr.fsize;

    /* void sfc_encryption_init(u32 app_addr, u32 sfc_base_addr, u32 unenc_start_addr); */
    /* sfc_encryption_init(boot_info.sfc.app_addr, boot_info.sfc.sfc_base_addr, boot_info.vm.vm_saddr); */

    device = dev_open(__SFC_NANE, 0);

    dev_ioctl(device, IOCTL_GET_CAPACITY, (u32)&capacity);
    boot_info.flash_size = capacity;
    log_info("boot info 0x%x\n", boot_info.flash_size);

    u8 uuid_buf[16];
    struct tzflash_ndec_cfg_data ndec_cfg;
    ndec_cfg.saddr = boot_info.vm.vm_saddr;
    ndec_cfg.eaddr = tzflash_cpu2flash_addr((void *)(0xfffffffUL));
    memcpy(&ndec_cfg.sfc, &boot_info.sfc, sizeof(struct sfc_info));
    dev_ioctl(device, IOCTL_SET_VM_INFO, (u32)&ndec_cfg);
    dev_ioctl(device, IOCTL_SET_PROTECT_INFO, (u32)flash_code_protect_callback);
    dev_ioctl(device, IOCTL_READ_FLASH_UUID, (u32)uuid_buf);
    dev_close(device);

    u32 sydf_get_top_area_info(const char *path, u32 * p_addr, u32 * p_len, u32 times);
    u32 p_addr;
    u32 p_len;
    int ret = sydf_get_top_area_info("otp_cfg", &p_addr, &p_len, 128);
    if (ret == 0) {
        /* log_info("otp addr:0x%x,len:%d", p_addr, p_len); */
        sydf_cfg.votp_cfg_addr = p_addr;
        sydf_cfg.votp_cfg_len = p_len;
    } else {
        sydf_cfg.votp_cfg_addr = 0;
        sydf_cfg.votp_cfg_len = 0;
    }
    ret = sydf_get_top_area_info("isd_config.ini", &p_addr, &p_len, 128);
    if (ret == 0) {
        /* log_info("ini addr:0x%x,len:%d", p_addr, p_len); */
        sydf_cfg.ini_cfg_addr = p_addr;
        sydf_cfg.ini_cfg_len = p_len;
    } else {
        sydf_cfg.ini_cfg_addr = 0;
        sydf_cfg.ini_cfg_len = 0;
    }

    return 0;
}
struct vfs_attr *get_vm_attr_p(void)
{
    return &vm_attr;
}

AT_SPI_CODE/*该函数放置段不可更改*/
u32 flash_code_protect_callback(u32 offset, u32 len)
{
    u32 limit_addr = vm_attr.sclust;
    /* log_info("0x%x 0x%x", limit_addr, offset); */
    if ((offset < limit_addr) || ((offset + len) > boot_info.flash_size)) {
        /* 超过正常擦写区域，不进行擦写操作 */
        return 1;
    } else {
        /* 进行擦写操作 */
        return 0;
    }
}

static struct vfs_attr eeprom_attr;
void vm_init_api(void)
{
    u32 err;
    void *pvfs = 0;
    void *pvfile = 0;
    void *device = 0;
    u32 capacity = 0;


    err = vfs_mount(&pvfs, (void *)NULL, (void *)NULL);
    ASSERT(!err, "fii vfs mount : 0x%x\n", err)
    err = vfs_openbypath(pvfs, &pvfile, "/app_area_head/EEPROM");
    ASSERT(!err, "fii vfs openbypath : 0x%x\n", err)
    err = vfs_ioctl(pvfile, FS_IOCTL_FILE_ATTR, (int)&eeprom_attr);
    ASSERT(!err, "fii vfs ioctl : 0x%x\n", err)
    log_info("EEPROM size : 0x%x\nEEPROM sclust : 0x%x\n", eeprom_attr.fsize, eeprom_attr.sclust);
    vfs_file_close(&pvfile);
    vfs_fs_close(&pvfs);

    syscfg_vm_init(eeprom_attr.sclust, eeprom_attr.fsize);
    /* *demo */
    /* u8 data_buf[10]; */

    /* for (int i = 0; i < sizeof(data_buf); i++) { */
    /* data_buf[i] = JL_RAND->R64L; */
    /* } */

    /* put_buf(data_buf, 10); */
    /* vm_write(VM_INDEX_DEMO, data_buf, 10); */

    /* memset(data_buf, 0x00, 10); */

    /* vm_read(VM_INDEX_DEMO, data_buf, 10); */

    /* put_buf(data_buf, 10); */

//   while (1);
}
struct vfs_attr *get_eeprom_attr_p(void)
{
    return &eeprom_attr;
}

sec_used(.version)
u8 const lib_update_version[] = "\x7c\x4f\x94\x0aUPDATE-@20210816-$9c89ae0";


/* void test_fs(void); */
/* void test_audio_dac(void); */
/* void clock_2_64m(void); */
void early_system_init(void)
{
    /* 该部分提前初始化为时钟初始化使用VM功能 */
    my_malloc_init();

    vfs_resource_init();
    vfs_init();

    flash_info_init();
    vm_init_api();
    tzflash_change_mode();
}

void system_init(void)
{
    /* clock_2_64m(); */
    d_key_voice_init();
    message_init();


    adc_init();
    app_power_init();

    tick_timer_init();

    key_init();

    devices_init();

    norfs_init_api();
    /* dac_mode_init(16, 0); */
    /* dac_power_on(SR_DEFAULT, 1); */
    /* test_audio_dac(); */
    audio_init();
    dac_mode_init(31, NULL);
    dac_init_api(SR_DEFAULT, 0);
    /* test_fs(); */
    d_mio_module_init();

    /* #if defined(UPDATE_V2_EN) && (1 == UPDATE_V2_EN) */
    //升级初始化
    int app_update_init(void);
    app_update_init();
    /* #endif */

}
#if 0
static void *t_pvfs = 0;
static void *t_pfile = 0;
static u8 t_buf[32];
void test_fs(void)
{
    u32 err;
    log_info("****************************************\n");
    err = vfs_mount(&t_pvfs, (void *)NULL, (void *)NULL);
    if (0 != err) {
        log_info(" err vfs mount 0x%x\n", err);
    }
    log_info("A 0x%x\n", (u32)t_pvfs);
    err = vfs_openbypath(t_pvfs, &t_pfile, "/dir_bin/");
    if (0 != err) {
        log_info(" err openbypath 0x%x\n", err);
    }
    log_info("B\n");
    err = vfs_openbyindex(t_pvfs, &t_pfile, 1);//SLEEP.lrc
    if (0 != err) {
        log_info(" err openbyindex 0x%x\n", err);
    }
    log_info("C\n");
    u32 len;
    len = vfs_read(t_pfile, t_buf, sizeof(t_buf));
    if (0 == len) {
        log_info("no data\n");
    }
    log_info_hexdump(t_buf, sizeof(t_buf));

}

void test_audio_dac(void)
{
    /* log_info("JL_CLOCK->CLK_CON2 : 0x%x", JL_CLOCK->CLK_CON2); */
    /* log_info("JL_APA->APA_CON0   : 0x%x", JL_APA->APA_CON0); */
    /* log_info("JL_APA->APA_CON1   : 0x%x", JL_APA->APA_CON1); */
}
#endif



