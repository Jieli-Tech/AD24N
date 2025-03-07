#include "common.h"
#include "uart.h"
#include "errno-base.h"
#include "vfs.h"
#include "boot.h"
#include "ioctl_cmds.h"
#include "device.h"
/* #include "mbr.h" */
#include "my_malloc.h"
#include "simple_fat/simple_fat.h"

extern u32 __dev_read(void *p, u8 *buf, u32 addr);
extern u32 __dev_write(void *p, u8 *buf, u32 addr);
#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

extern FIL pfi;
/****************api*****************/
u32 smpl_fat_monut_api(void **ppfs, void *p_device)
{
    if ((u8 *)p_device  == NULL) {
        return -1;
    }

    return fat_init((u8 *)p_device);
}


u32 smpl_fat_openR_api(void *pfs, void **ppfile, const char *path)
{
    return f_open(&pfi, (char *)path, FA_OPEN_EXISTING);
}

u32 smpl_fat_read_api(void *pfile, void *buff, u32 len)
{
    u32 rlen = f_read(&pfi, buff, len);
    /* if (rlen != len) { */
    /* return 0; */
    /* } */
    return rlen;
}

u32 smpl_fat_seek_api(void *pfile, u32 offset, u32 fromwhere)
{
    return f_seek(&pfi, offset);
}

u32 smpl_fat_file_close_api(void **ppfile)
{
    return (u32) * ppfile;
}

u32 smpl_fat_fs_close_api(void **ppfs)
{
    return (u32) * ppfs;
}

/* #include "vfs.h" */
/* u32 fat_get_attrs(void *p_f_hdl, struct vfs_attr *attr); */
/* int smpl_fat_attrs_api(void *pfile, void *attr) */
/* { */
/*     return fat_get_attrs(pfile, (struct vfs_attr *)attr); */
/* } */

const struct vfs_operations smpl_fat_vfs_ops sec_used(.vfs_operations) = {
    .fs_type = "simple_fat",
    /* .init        = fat_init_api, */
    .mount       = smpl_fat_monut_api,
    .openbypath  = smpl_fat_openR_api,
    .createfile  = NULL,
    .read        = smpl_fat_read_api,
    .write       = NULL,
    .seek        = smpl_fat_seek_api,
    .close_fs 	 = smpl_fat_fs_close_api,
    .close_file  = smpl_fat_file_close_api,
    /* .fget_attr   = smpl_fat_attrs_api, */
    .fget_attr   = NULL,
    .name        = NULL,
    .flen        = NULL,
    .ftell       = NULL,
    .openbyindex = NULL,
    .openbyclust = NULL,
    .openbyfile  = NULL,
    /* .fscan       = NULL, */
    .ioctl       = NULL,
};
#if 0
void simple_fat_test(u32 seek)
{
    log_info("simple_fat_test\n");
    void *pvfs = 0;
    void *pvfile = 0;
    void *device = 0;
    u32 err;
    u8 demo_buff[64] = {0};

    /* 1. 打开SD卡设备 */
    device = dev_open(__SD0_NANE, 0);
    if (device == NULL) {
        log_info("dev null !!!! \n");
        return;
    }

    /* 2. 挂载simple_fat文件系统 */
    log_info("mount-------\n");
    err = vfs_mount(&pvfs, device, "simple_fat");
    if (err != 0) {
        log_info("dev mount error 0x%x !!! \n", err);
        dev_close(device);
        return;
    }

    /* 3. 打开dir_f2a.txt文件 */
    log_info("openbypath-------\n");
    err = vfs_openbypath(pvfs, &pvfile, "/dir_f2a.txt");
    if (err) {
        log_info("openbypath 0x%x\n", err);
        vfs_fs_close(&pvfs);
    }
    /* 4. 如果有偏移，则偏移seek */
    if (seek) {
        err = vfs_seek(pvfile, seek, 0);
        if (err) {
            log_info("seek ret 0x%x\n", err);
        }
    }
    /* 5. 数据读取 */
    log_info("vfs_read--------\n");
    vfs_read(pvfile, demo_buff, 64);
    log_info_hexdump(demo_buff, 64);
    vfs_file_close(&pvfile);
    vfs_fs_close(&pvfs);
}
#endif

