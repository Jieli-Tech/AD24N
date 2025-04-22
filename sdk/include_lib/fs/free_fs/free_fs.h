/***********************************Jieli tech************************************************
  File : free_fs.h
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2025-02-26
********************************************************************************************/
#ifndef _FREE_FS_H_
#define _FREE_FS_H_

// #include "sdk_cfg.h"
#include "typedef.h"




//文件系统句柄
typedef struct __FREE_FILESYSTEM {
    void	*hdev;
    u32 capacity;
} FREE_FILESYSTEM;

//文件句柄
typedef struct __FREE_FS_FILE {
    FREE_FILESYSTEM *pfs;
    u32 rw_p;
    u32 len;
    u32 offset;
    u32 index;
    // char name[16];
} FREE_FS_FILE;


u32 freefs_mount(FREE_FILESYSTEM **ppfs, void *p_device);
u32 freefs_openbyindex(FREE_FILESYSTEM *pfs, FREE_FS_FILE **ppfile, u32 index);
u16 freefs_write(FREE_FS_FILE *pfile, u8 *buff, u16 len);
u16 freefs_read(FREE_FS_FILE *pfile, u8 *buff, u16 len);
u32 freefs_seek(FREE_FS_FILE *pfile, u32 type, u32 offsize);
u32 freefs_closefile(FREE_FS_FILE **ppfile);
u32 freefs_close(FREE_FILESYSTEM **ppfs);
u32 freefs_file_name(FREE_FS_FILE *pfile, char *name, u32 len);







#endif
