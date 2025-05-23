#ifndef __VM_API_H__
#define __VM_API_H__
#include "typedef.h"
#include "ioctl.h"
#include "dev_mg/device.h"


typedef enum {
    VM_INDEX_DEMO = 0,

    //================系统内部 ，预留32个id，不可修改顺序===========================//
    /*RTC*/
    LIB_VM_INDEX_LP_SUM_CNT     = 1,
    LIB_VM_INDEX_LP_LAST_CNT    = 2,
    LIB_VM_INDEX_RTC_TIME       = 3,
    LIB_VM_INDEX_ALM_TIME       = 4,
    /*PMU VOLTAGE*/
    LIB_VM_PMU_VOLTAGE          = 5,    //电压检测
    /*AUDIO_VBG*/
    VM_INDEX_AUDIO_VBG_TRIM     = 6,    //AUDIO_VBG

    /*PLL_LDO*/
    LIB_VM_PLL_LDO              = 7,

    LIB_SYSMEM_END              = 32,
    // 系统lib使用结束
    //================================================================================//

    //================================================================================//
    // 以下用户可以任意修改顺序或添加
    VM_INDEX_SONG,
    VM_INDEX_ENG,
    VM_INDEX_POETRY,
    VM_INDEX_STORY,
    VM_INDEX_F1X,
    VM_INDEX_EXT_SONG,
    VM_INDEX_VOL,

#if 1
    VM_INDEX_SONG_BP,
    VM_INDEX_ENG_BP,
    VM_INDEX_POETRY_BP,
    VM_INDEX_STORY_BP,
    VM_INDEX_F1X_BP,
    VM_INDEX_EXT_SONG_BP,
#endif

    /*mbox*/
    VM_INDEX_SYSMODE,
    VM_INDEX_UDISK_BP,
    VM_INDEX_UDISK_INDEX,
    VM_INDEX_SDMMCA_BP,
    VM_INDEX_SDMMCA_INDEX,
    VM_INDEX_SDMMCB_BP,
    VM_INDEX_SDMMCB_INDEX,
    VM_INDEX_EXTFLSH_BP,
    VM_INDEX_EXTFLSH_INDEX,
    VM_INDEX_ACTIVE_DEV,
    VM_INDEX_FM_FRE,
    VM_INDEX_FM_CHAN,
    VM_INDEX_FM_CHANNL,//VM_INDEX_FM_CHANNL和VM_INDEX_FM_CHANNL_END之间不能插入其它INDEX
    VM_INDEX_FM_CHANNL_END = VM_INDEX_FM_CHANNL + 28,
    VM_INDEX_AUTO_BP,
} VM_INDEX;

int syscfg_vm_init(u32 mem_addr, u32 mem_size);
int vm_read(u32 id, u8 *data_buf, u16 len);
int vm_write(u32 id, u8 *data_buf, u16 len);
void vm_pre_erase(void);

// 驱动内部使用接口
int syscfg_write(u16 item_id, const void *buf, u16 len);
int syscfg_read(u16 item_id, void *buf, u16 len);

#endif
