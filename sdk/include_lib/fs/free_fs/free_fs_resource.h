#ifndef _free_FS_RESOURCE_H
#define _free_FS_RESOURCE_H

#include "free_fs/free_fs.h"


FREE_FILESYSTEM *freefs_fshdl_malloc(void);
FREE_FILESYSTEM *freefs_fshdl_free(FREE_FILESYSTEM *pfs);
FREE_FS_FILE *freefs_filehdl_malloc(void);
FREE_FS_FILE *freefs_filehdl_free(FREE_FS_FILE *pfile);

#endif
