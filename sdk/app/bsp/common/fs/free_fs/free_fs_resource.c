#include "free_fs/free_fs.h"
/* #include "common.h" */
/* #include "uart.h" */
/* #include "errno-base.h" */
#include "vfs.h"
/* #include "boot.h" */
#include "app_modules.h"
#include "my_malloc.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[free_fs]"
#include "log.h"

#if defined(FREE_FS_EN) && (FREE_FS_EN)

FREE_FILESYSTEM *freefs_fshdl_malloc(void)
{
    return my_malloc(sizeof(FREE_FILESYSTEM), MM_FREEFS);
}

FREE_FILESYSTEM *freefs_fshdl_free(FREE_FILESYSTEM *pfs)
{
    return my_free(pfs);
}

FREE_FS_FILE *freefs_filehdl_malloc(void)
{
    return my_malloc(sizeof(FREE_FS_FILE), MM_FREEFF);
}

FREE_FS_FILE *freefs_filehdl_free(FREE_FS_FILE *pfile)
{
    return my_free(pfile);
}


void freefs_init_api(void)
{
}
//-----------------------------FREE FS API

u32 freefs_monut_api(void **ppfs, void *p_device)
{
    return freefs_mount((FREE_FILESYSTEM **)ppfs, (void *)p_device);
}

u32 freefs_openbyindex_api(void *pfs, void **ppfile, u32 index)
{
    return freefs_openbyindex((FREE_FILESYSTEM *)pfs, (FREE_FS_FILE **)ppfile, index);
}

u32 freefs_createfile_api(void *pfs, void **ppfile, u32 *pindex)
{
    return 0;
    /* return freefs_createfile((FREE_FILESYSTEM *)pfs, (FREE_FS_FILE **)ppfile, pindex); */
}

u32 freefs_write_api(void *pfile, void *buff, u32 len)
{
    return (u32)freefs_write((FREE_FS_FILE *)pfile, (u8 *)buff, len);
}

u32 freefs_read_api(void *pfile, void *buff, u32 len)
{
    return (u32)freefs_read((FREE_FS_FILE *)pfile, (u8 *)buff, len);
}

u32 freefs_seek_api(void *pfile, u32 offset, u32 fromwhere)
{
    return (u32)freefs_seek((FREE_FS_FILE *)pfile, (u8)fromwhere, offset);
}


u32 freefs_file_close_api(void **ppfile)
{
    return (u32)freefs_closefile((FREE_FS_FILE **)ppfile);
}

u32 freefs_close_api(void **ppfs)
{
    return freefs_close((FREE_FILESYSTEM **)ppfs);
}

int freefs_ioctl_api(void *pfile, int cmd, int arg)
{
    return 0;
    /* return freefs_ioctl((FREE_FS_FILE *)pfile, cmd, arg); */

}

u32 freefs_name_api(void *pfile, void *name, u32 len)
{
    return freefs_file_name((FREE_FS_FILE *)pfile, (char *)name, len);
}

//REGISTER_VFS_OPERATIONS(sydfvfs_ops) = {
const struct vfs_operations freefs_vfs_ops sec_used(.vfs_operations) = {
    .fs_type = "freefs",
    /* .init        = freefs_init_api, */
    .init        = NULL,
    .mount       = freefs_monut_api,
    .openbypath  = NULL,
    .openbyindex = freefs_openbyindex_api,
    .createfile  = freefs_createfile_api,
    .read        = freefs_read_api,
    .write       = freefs_write_api,
    .seek        = freefs_seek_api,
    .close_fs 	 = freefs_close_api,
    .close_file  = freefs_file_close_api,
    .fget_attr  = NULL,
    .name        = freefs_name_api,
    .ioctl       = freefs_ioctl_api,
};

#endif
