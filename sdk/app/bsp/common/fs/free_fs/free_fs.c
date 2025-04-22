/***********************************Jieli tech************************************************
  File : free_fs.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2025-02-27
********************************************************************************************/
#include "free_fs/free_fs.h"
#include "free_fs/free_fs_resource.h"
#include "common.h"
/* #include "uart.h" */
#include "errno-base.h"
#include "vfs.h"
#include "device.h"
#include "app_modules.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[free_fs]"
#include "log.h"



#if defined(FREE_FS_EN) && (FREE_FS_EN)

static const u32 free_file_addr_tab[] = {
    0,
    1024 * 4,
    1024 * 8
};

#define D_FREE_FILE_MAX (sizeof(free_file_addr_tab) / sizeof(free_file_addr_tab[0]))


void freefs_mutex_enter(void)      ///<申请信号量
{

}

void freefs_mutex_exit(void)       ///<释放信号量
{

}

/*----------------------------------------------------------------------------*/
/**@brief   提供给FREE文件系统的物理擦除函数，4K单位
   @param   hdev: 设备句柄
   @param   addr: 设备地址。
   @author  liujie
   @note    void phy_freefs_eraser(void *hdev, u32 addr)
*/
/*----------------------------------------------------------------------------*/
static void phy_freefs_eraser(void *hdev, u32 addr)
{
    /* log_info(" nor eraser : 0x%x\n", addr); */
    dev_ioctl(hdev, IOCTL_ERASE_SECTOR, addr);
}

/*----------------------------------------------------------------------------*/
/**@brief   提供给FREE文件系统的物理读函数
   @param   addr:读取的地址，字节单位。
   @param   len:读取的长度，字节单位。
   @param   buf:读取的目标BUFF。
   @return  u16:读取的长度
   @author  liujie
   @note    u8 phy_freefs_read(u32 addr,u8 *buf,u16 len)
*/
/*----------------------------------------------------------------------------*/
static u16 phy_freefs_read(FREE_FILESYSTEM *pfs, u32 addr, u8 *buf, u16 len)
{
    /* log_info(" freefs read : 0x%x %d\n", addr, len); */
    freefs_mutex_enter();
    u32 capacity = addr + len;
    len = pfs->capacity >  capacity ? len : (pfs->capacity - addr);
    dev_byte_read(pfs->hdev, buf, addr, len);
    freefs_mutex_exit();
    return len;
}

/*----------------------------------------------------------------------------*/
/**@brief   提供给free文件系统的物理写函数
   @param   addr:要写入设备的地址，字节单位。
   @param   len:需要写入的长度，字节单位。
   @param   buf:数据来源BUFF。
   @return  u16:写入的长度
   @author  liujie
   @note    u16 freefs_wirte(u32 addr,u8 *buf,u16 len)
*/
/*----------------------------------------------------------------------------*/
static u16 phy_freefs_wirte(FREE_FILESYSTEM *pfs, u32 addr, u8 *buf, u16 len)
{
    /* log_info(" freefs write : 0x%x %d\n", addr, len); */
    freefs_mutex_enter();
    u32 capacity = addr + len;
    if (0 == len) {
        return len;
    }

    len = pfs->capacity >  capacity ? len : (pfs->capacity - addr);

    if (0 == (addr % (4 * 1024))) {
        phy_freefs_eraser(pfs->hdev, addr);
    }
    u32 sector_start = addr / (4 * 1024);
    u32 sector_end = (addr + len - 1) / (4 * 1024);
    u32 eraser_addr = addr;
    for (u32 i = sector_start + 1; i <= sector_end; i++) {
        eraser_addr += (4 * 1024);
        phy_freefs_eraser(pfs, eraser_addr);
    }
    len = dev_byte_write(pfs->hdev, buf, addr, len);
    freefs_mutex_exit();
    return len;
}


/*----------------------------------------------------------------------------*/
/**@brief   FREE文件系统的挂载函数
   @param   ppfs: 指向文件系统句柄指针的指针
   @param   p_device: 设备句柄。
   @return  u32：错误类型
   @author  liujie
   @note    u32 freefs_mount(FREE_FILESYSTEM **ppfs, void *p_device)
*/
/*----------------------------------------------------------------------------*/
u32 freefs_mount(FREE_FILESYSTEM **ppfs, void *p_device)
{
    if (p_device == NULL) {
        return E_DEV_NULL;
    }
    /* log_info("freefs_mount !!!\n"); */
    if (*ppfs == (void *)NULL) {
        *ppfs = freefs_fshdl_malloc();
        if (*ppfs == (void *)NULL) {
            return E_PFS_NULL;
        }

    }
    FREE_FILESYSTEM *pfs;
    pfs = *ppfs;
    pfs->hdev = p_device;
    dev_ioctl(pfs->hdev, IOCTL_GET_CAPACITY, (u32)&pfs->capacity);
    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   FREE文件系统,按文件号打开文件
   @param   pfs: 指向文件系统句柄的指针
   @param   ppfile: 指向文件句柄指针的指针。
   @param   index: 文件号。
   @return  u32：错误类型
   @author  liujie
   @note    u32 freefs_openbyindex(FREE_FILESYSTEM *pfs, FREE_FS_FILE **ppfile, u32 index)
*/
/*----------------------------------------------------------------------------*/
u32 freefs_openbyindex(FREE_FILESYSTEM *pfs, FREE_FS_FILE **ppfile, u32 index)
{
    if ((index == 0) || (index > D_FREE_FILE_MAX)) {
        return E_FILEINDEX;
    }
    FREE_FS_FILE *pfile = 0;
    if ((FREE_FS_FILE *)NULL == *ppfile) {
        *ppfile = freefs_filehdl_malloc();
        if ((void *)NULL == *ppfile) {
            return E_PFILE_NULL;
        }
    }
    pfile = *ppfile;
    memset((void *) pfile, 0, sizeof(FREE_FS_FILE));
    pfile->pfs = pfs;
    pfile->index = index;
    pfile->offset = free_file_addr_tab[index - 1];
    if (index == D_FREE_FILE_MAX) {
        pfile->len = pfs->capacity - free_file_addr_tab[index - 1];
    } else {
        pfile->len = free_file_addr_tab[index] - free_file_addr_tab[index - 1];
    }
    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   FREE文件系统，文件写函数
   @param   pfile: 指向文件句柄的指针。
   @param   buff: 存放写数据的缓存。
   @param   len：需要写的数据的字节长度。
   @return  u32：成功写入的字节长度
   @author  liujie
   @note    u16 freefs_write(FREE_FS_FILE *pfile, u8 *buff, u16 len)
*/
/*----------------------------------------------------------------------------*/
u16 freefs_write(FREE_FS_FILE *pfile, u8 *buff, u16 len)
{

    u32 dev_addr = pfile->offset + pfile->rw_p;
    u16 res_len = phy_freefs_wirte(pfile->pfs, dev_addr, buff, len);
    pfile->rw_p += res_len;
    pfile->len += res_len;
    return res_len;
}

/*----------------------------------------------------------------------------*/
/**@brief   FREE文件系统，文件读函数
   @param   pfile: 指向文件句柄的指针。
   @param   buff: 存放读数据的缓存。
   @param   len：需要读的数据的字节长度。
   @return  u32：成功读取的字节长度
   @author  liujie
   @note    u16 freefs_read(FREE_FS_FILE *pfile, u8 *buff, u16 len)
*/
/*----------------------------------------------------------------------------*/
u16 freefs_read(FREE_FS_FILE *pfile, u8 *buff, u16 len)
{

    u32 dev_addr = pfile->offset + pfile->rw_p;
    u32 res_len = phy_freefs_read(pfile->pfs, dev_addr, buff, len);
    pfile->rw_p += res_len;
    pfile->len += res_len;
    return res_len;
}
/*----------------------------------------------------------------------------*/
/**@brief   文件seek函数
   @param   pfile:文件句柄
   @param   type:seek的格式
   @param   offsize:seek的偏移量。
   @return  u8:返回值
   @author  liujie
   @note    u8 norfs_seek (REC_FILE *pfile, u8 type, u32 offsize)
*/
/*----------------------------------------------------------------------------*/

u32 freefs_seek(FREE_FS_FILE *pfile, u32 type, u32 offsize)
{
    u32 file_len;
    if (NULL == pfile) {
        return 0;
    }

    file_len = pfile->len;

    switch (type) {
    case SEEK_SET:
        if (offsize >= file_len) {
            return E_SEEK_END;
        }
        pfile->rw_p = offsize;
        break;
    case SEEK_CUR:
        pfile->rw_p += offsize;
        break;
    case SEEK_END:
        if (offsize > file_len) {
            return E_SEEK_START;
        }
        pfile->rw_p = file_len - offsize;
        break;
    }
    return 0;
}


/*----------------------------------------------------------------------------*/
/**@brief   FREE文件系统，文件读函数
   @param   ppfile: 指向文件句柄指针的指针。
   @return  u32：错误类型
   @author  liujie
   @note    u32 freefs_closefile(FREE_FS_FILE **ppfile)
*/
/*----------------------------------------------------------------------------*/
u32 freefs_closefile(FREE_FS_FILE **ppfile)
{
    *ppfile = freefs_filehdl_free(*ppfile);
    return 0;
}


/*----------------------------------------------------------------------------*/
/**@brief   FREE文件系统，关闭文件系统
   @param   ppfs: 指向文件系统句柄指针的指针。
   @return  u32：错误类型
   @author  liujie
   @note
*/
/*----------------------------------------------------------------------------*/
u32 freefs_close(FREE_FILESYSTEM **ppfs)
{
    *ppfs =  freefs_fshdl_free(*ppfs);
    return (u32) * ppfs;
}

/*----------------------------------------------------------------------------*/
/**@brief   FREE文件系统，获取文件名
   @param   pfile: 指向文件句柄的指针。
   @param   name: 文件名buf。
   @param   len: name的最大长度。
   @return  u32：文件名长度
   @author  liujie
   @note    u32 freefs_file_name(FREE_FS_FILE *pfile, char *name, u32 len)
*/
/*----------------------------------------------------------------------------*/
u32 freefs_file_name(FREE_FS_FILE *pfile, char *name, u32 len)
{
    memcpy(name, "free file", len);
    return len;
}


#endif
