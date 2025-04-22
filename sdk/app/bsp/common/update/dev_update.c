#include "code_v2/update.h"
#include "code_v2/update_loader_download.h"
#include "code_v2/dev_update.h"
/* #include "dev_manager.h" */
#include "vfs.h"
#include "app_config.h"
/* #include "btcontroller_modules.h" */
#include "device.h"
/* #include "sd.h" */
#include "sdmmc/sd_host_api.h"
#include "gpio.h"
#include "fat/fat_io_new.h"
#include "asm/power/power_reset.h"
#include "init.h"

#define LOG_TAG_CONST       UPDATE
#define LOG_TAG             "[dev_update]"
#include "log.h"
/* #include "trim.h" */


////////////////////////////////////////////
#define fopen vfs_openbypath
#define fread vfs_read
#define fseek vfs_seek
#define fclose vfs_file_close
#define TCFG_SD0_ENABLE 1
#define TCFG_SD1_ENABLE 0
#define TCFG_SD1_ENABLE 0

#define TCFG_SD0_PORT_CLK SDMMC_CLK_IO
#define TCFG_SD0_PORT_CMD SDMMC_CMD_IO
#define TCFG_SD0_PORT_DA0 SDMMC_DAT_IO

////////////////////////////////////////////


#if defined(SD_UPDATE_EN) || defined(UDISK_UPDATE_EN)
#define DEV_UPDATE_EN		1
#else
#define DEV_UPDATE_EN		0
#endif


extern bool uart_update_send_update_ready(char *file_path);
extern bool get_uart_update_sta(void);
/* extern void latch_reset(struct boot_soft_flag_t *softflag); */

static char update_path[48] = {0};
extern const char updata_file_name[];

struct __update_dev_reg {
    char *logo;
    int type;
    union {
        UPDATA_SD sd;
        UPDATE_UDISK udisk;
    } u;
};


#if TCFG_SD0_ENABLE
static const struct __update_dev_reg sd0_update = {
    .logo = "sd0",
    .type = SD0_UPDATA,
    .u.sd.control_type = SD_CONTROLLER_0,
#if 1//def TCFG_SD0_PORT_CMD
    .u.sd.control_io_clk = TCFG_SD0_PORT_CLK,
    .u.sd.control_io_cmd = TCFG_SD0_PORT_CMD,
    .u.sd.control_io_dat = TCFG_SD0_PORT_DA0,
#else
#if (TCFG_SD0_PORTS=='A')
    .u.sd.control_io = SD0_IO_A,
#elif (TCFG_SD0_PORTS=='B')
    .u.sd.control_io = SD0_IO_B,
#elif (TCFG_SD0_PORTS=='C')
    .u.sd.control_io = SD0_IO_C,
#elif (TCFG_SD0_PORTS=='D')
    .u.sd.control_io = SD0_IO_D,
#elif (TCFG_SD0_PORTS=='E')
    .u.sd.control_io = SD0_IO_E,
#elif (TCFG_SD0_PORTS=='F')
    .u.sd.control_io = SD0_IO_F,
#endif
#endif
    .u.sd.power = 1,
};
#endif//TCFG_SD0_ENABLE

#if TCFG_SD1_ENABLE
static const struct __update_dev_reg sd1_update = {
    .logo = "sd1",
    .type = SD1_UPDATA,
    .u.sd.control_type = SD_CONTROLLER_1,
#if (TCFG_SD1_PORTS=='A')
    .u.sd.control_io = SD1_IO_A,
#else
    .u.sd.control_io = SD1_IO_B,
#endif
    .u.sd.power = 1,

};
#endif//TCFG_SD1_ENABLE

#if TCFG_UDISK_ENABLE
static const struct __update_dev_reg udisk_update = {
    .logo = "udisk0",
    .type = USB_UPDATA,
};
#endif//TCFG_UDISK_ENABLE


static const struct __update_dev_reg *update_dev_list[] = {
#if TCFG_UDISK_ENABLE
    &udisk_update,
#endif//TCFG_UDISK_ENABLE
#if TCFG_SD0_ENABLE
    &sd0_update,
#endif//
#if TCFG_SD1_ENABLE
    &sd1_update,
#endif//TCFG_SD1_ENABLE
};

void *dev_update_get_parm(int type)
{
    struct __update_dev_reg *parm = NULL;
    for (int i = 0; i < ARRAY_SIZE(update_dev_list); i++) {
        if (update_dev_list[i]->type == type) {
            parm = (struct __update_dev_reg *)update_dev_list[i];
        }
    }

    if (parm == NULL) {
        return NULL;
    }
    return (void *)&parm->u.sd;
}


struct strg_update {
    void *pfs;
    void *fd;
    char *update_path;
};
static struct strg_update strg_update = {0};
#define __this 		(&strg_update)

static u16 strg_f_open(void)
{
    if (!__this->update_path) {
        log_info("file path err ");
        return false;
    }

    if (__this->fd) {
        return true;
        /* fclose(__this->fd);
        __this->fd = NULL; */
    }
    /* __this->fd = fopen(__this->update_path, "r"); */
    fopen(__this->pfs, &__this->fd, __this->update_path);
    if (!__this->fd) {
        log_info("file open err ");
        return false;
    }
    return true;
}

static u16 strg_f_read(void *fp, u8 *buff, u16 len)
{
    if (!__this->fd) {
        return (u16) - 1;
    }

    len = fread(__this->fd, buff, len);
    return len;
}

static int strg_f_seek(void *fp, u8 type, u32 offset)
{
    if (!__this->fd) {
        return (int) - 1;
    }

    int ret = fseek(__this->fd, offset, type);
    /* return 0; // 0k */
    return ret;
}
static u16 strg_f_stop(u8 err)
{
    log_info("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
    if (__this->fd) {
        fclose(&__this->fd);
        __this->fd = NULL;
    }
    return true;
}

/* static int strg_update_set_file_path_and_hdl(char *update_path, void *fd) */
static int strg_update_set_file_path_and_hdl(char *update_path, void *fd, void *pfs)
{
    __this->update_path = update_path;
    __this->fd = fd;
    __this->pfs = pfs;

    return true;
}

static const update_op_api_t strg_dev_update_op = {
    .f_open = strg_f_open,
    .f_read = strg_f_read,
    .f_seek = strg_f_seek,
    .f_stop = strg_f_stop,
};

static void dev_update_param_private_handle(UPDATA_PARM *p)
{

#if defined(SD_UPDATE_EN) && defined(UDISK_UPDATE_EN)
    u16 up_type = p->parm_type;
#endif


#ifdef SD_UPDATE_EN
    if ((up_type == SD0_UPDATA) || (up_type == SD1_UPDATA)) {
        int sd_start = (u32)p->parm_priv;
        void *sd = NULL;
        sd = dev_update_get_parm(up_type);
        if (sd) {
            memcpy((void *)sd_start, sd, UPDATE_PRIV_PARAM_LEN);
        } else {
            memset((void *)sd_start, 0, UPDATE_PRIV_PARAM_LEN);
        }
    }
#endif

#ifdef UDISK_UPDATE_EN
    if (up_type == USB_UPDATA) {
        log_info("usb updata ");
        UPDATE_UDISK *usb_start = (UPDATE_UDISK *)p->parm_priv;
        memset((void *)usb_start, 0, UPDATE_PRIV_PARAM_LEN);
        void *usb = dev_update_get_parm(up_type);
        if (usb) {
            memcpy((void *)usb_start, usb, UPDATE_PRIV_PARAM_LEN);
#ifdef CONFIG_CPU_BR27
            usb_start->u.param.rx_ldo_trim = trim_get_usb_rx_ldo();
            usb_start->u.param.tx_ldo_trim = trim_get_usb_tx_ldo();
#endif
        }
    }
#endif
    memcpy(p->file_patch, updata_file_name, strlen(updata_file_name));
#if UPDATA_KEEP_IO_ENABLE
    update_keep_io(p);
#endif
}

void dev_update_before_jump_handle(int up_type)
{
    log_info(">>>[test]:dev latch reset update\n");

    common_update_before_jump_reset_handle();
}

static void dev_update_state_cbk(int type, u32 state, void *priv)
{
    update_ret_code_t *ret_code = (update_ret_code_t *)priv;

    switch (state) {
    case UPDATE_CH_EXIT:
        if ((0 == ret_code->stu) && (0 == ret_code->err_code)) {
            update_mode_api_v2(type,
                               dev_update_param_private_handle,
                               dev_update_before_jump_handle);
        } else {
#if UPDATA_KEEP_IO_ENABLE
            log_info("update fail, latch reset!!!\n");
            update_get_latch_romio(NULL);
            common_update_before_jump_reset_handle();
#else
            log_info("update fail, cpu reset!!!\n");
            system_reset(UPDATE_FLAG);
#endif
        }
        break;
    }
}

#if 0
u16 dev_update_check(char *logo)
{
    if ((update_success_boot_check() == true) || !CONFIG_UPDATE_STORAGE_DEV_EN) {
        return UPDATA_NON;
    }
    struct __dev *dev = dev_manager_find_spec(logo, 0);
    if (dev) {
#if DEV_UPDATE_EN
        //<查找设备升级配置
        struct __update_dev_reg *parm = NULL;
        for (int i = 0; i < ARRAY_SIZE(update_dev_list); i++) {
            if (0 == strcmp(update_dev_list[i]->logo, logo)) {
                parm = (struct __update_dev_reg *)update_dev_list[i];
            }
        }
        if (parm == NULL) {
            log_info("dev update without parm err!!!\n");
            return UPDATA_PARM_ERR;
        }

        //最新设备升级只支持fat32(省flash空间)，此处截断
        struct vfs_partition *part = fget_partition((const char *)dev_manager_get_root_path(dev));
        if (part) {
            // fs_type: 1为fat12 、2 为fat16 、3为fat32 、4为exfat
            if (part->fs_type != 3) {
                log_info(">>>[test]:dev update only support fat32 !!!\n");
                return UPDATA_PARM_ERR;
            }
        } else {
            log_info(">>>[test]:dev part err!!!\n");
            return UPDATA_PARM_ERR;
        }

        //<尝试按照路径打开升级文件
        char *updata_file = (char *)updata_file_name;
        if (*updata_file == '/') {
            updata_file ++;
        }
        memset(update_path, 0, sizeof(update_path));
        log_info(update_path, "%s%s", dev_manager_get_root_path(dev), updata_file);
        log_info("update_path: %s\n", update_path);
        FILE *fd = fopen(update_path, "r");
        if (!fd) {
            //没有升级文件， 继续跑其他解码相关的流程
            log_info("open update file err!!!\n");
            return UPDATA_DEV_ERR;
        }

#if(TCFG_UPDATE_UART_IO_EN) && (TCFG_UPDATE_UART_ROLE)
        uart_update_send_update_ready(update_path);
        while (get_uart_update_sta()) {
            os_time_dly(10);
        }
#else
        //进行升级
        strg_update_set_file_path_and_hdl(update_path, (void *)fd);

        update_mode_info_t info = {
            .type = parm->type,
            .state_cbk = dev_update_state_cbk,
            .p_op_api = &strg_dev_update_op,
            .task_en = 0,
        };
        app_active_update_task_init(&info);

#endif
#endif//DEV_UPDATE_EN
    }
    return UPDATA_READY;
}
#else

extern u32 d_new_stack_end[];
extern u32 d_new_stack_start[];
u16 dev_update_check(char *logo, bool check_flag)
{
    /* if ((update_success_boot_check() == true) || !CONFIG_UPDATE_STORAGE_DEV_EN) { */
    /*     return UPDATA_NON; */
    /* } */

    /* if (!CONFIG_UPDATE_STORAGE_DEV_EN) { */
    /*     return UPDATA_NON; */
    /* } */

    log_info(">>>[test]:----------------dev_update_check----logo = %s------------\n", logo);
    void force_set_sd_online(char *sdx);
    /* force_set_sd_online(logo); */
    void *device = dev_open(logo, NULL);
    /* void *pfs = __this->pfs, *pfile = __this->fd; */
    static void *pfs = NULL, *pfile = NULL;
    y_printf(">>>[test]:device = 0x%x\n", device);
    if (device) {
#if DEV_UPDATE_EN
        log_info("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
        //<查找设备升级配置
        struct __update_dev_reg *parm = NULL;
        for (int i = 0; i < ARRAY_SIZE(update_dev_list); i++) {
            if (0 == strcmp(update_dev_list[i]->logo, logo)) {
                parm = (struct __update_dev_reg *)update_dev_list[i];
            }
        }
        if (parm == NULL) {
            log_info("dev update without parm err!!!\n");
            dev_close(device);
            return UPDATA_PARM_ERR;
        }
        u32 res = vfs_mount(&pfs, device, DUFAULT_FAT);
        if (res != 0) {
            log_info("dev mount error 0x%x !!! \n", res);
            dev_close(device);
            return UPDATA_PARM_ERR;
        }

#if 0
        FS_PARTITION_INFO part_info = {0};
        res = vfs_ioctl(pfs, (int)FS_IOCTL_GET_PARTITION_INFO, (int)&part_info);
        if (res) {
            log_info(">>>[test]:dev part err!!!\n");
            vfs_fs_close(&pfs);
            dev_close(device);
            return UPDATA_PARM_ERR;
        } else {
            // fs_type: 1为fat12 、2 为fat16 、3为fat32 、4为exfat
            if (part_info.fs_type != 3) {
                log_info(">>>[test]:dev update only support fat32 !!!\n");
                vfs_fs_close(&pfs);
                dev_close(device);
                return UPDATA_PARM_ERR;
            }
        }
#endif

        //<尝试按照路径打开升级文件
        memcpy(update_path, updata_file_name, strlen(updata_file_name));
        /* FILE *fd = fopen(update_path, "r"); */
        vfs_openbypath(pfs, &pfile, update_path);
        if (!pfile) {
            //没有升级文件， 继续跑其他解码相关的流程
            log_info("open update file err!!!\n");
            vfs_fs_close(&pfs);
            dev_close(device);
            return UPDATA_DEV_ERR;
        }
        if (check_flag) {
            vfs_file_close(&pfile);
            vfs_fs_close(&pfs);
            dev_close(device);
            return UPDATA_READY;
        }
        /* #if(TCFG_UPDATE_UART_IO_EN) && (TCFG_UPDATE_UART_ROLE) */
        /*         uart_update_send_update_ready(update_path); */
        /*         while (get_uart_update_sta()) { */
        /*             os_time_dly(10); */
        /*         } */
        /* #else */
        //进行升级
        strg_update_set_file_path_and_hdl(update_path, (void *)pfile, (void *)pfs);
        update_mode_info_t info = {
            .type = parm->type,
            .state_cbk = dev_update_state_cbk,
            .p_op_api = &strg_dev_update_op,
            .task_en = 0,
        };
#if 1
        /* log_info("1addr 0x%x\n", &info); */
        /* put_buf((u8 *)&info, sizeof(info)); */
        local_irq_disable(); //第一次关
        /* 切栈后的应用流程, stack_change_step*/
        /* stack_change_step 1:关闭堆栈保护，然后开始切换堆栈。该流程需要在关中断之后执行 */
        stack_protect_set(0, (u32)&_cpu0_sstack_begin, (u32)&_cpu0_sstack_end);
        call_by_change_stack((u32)&d_new_stack_end[0], (void *)((u32)dev_update_by_change_stack), (void *)&info);

        /* stack_change_step 5:切栈后的应用执行完成后，call_by_change_stack函数会恢复原本的堆栈，但还需要用户自行恢复堆栈保护 */
        stack_protect_set(1, (u32)&_cpu0_sstack_begin, (u32)&_cpu0_sstack_end);
        local_irq_enable();//第二次恢复
#else
        app_active_update_task_init(&info);
#endif
        /* #endif */
#endif
    }
    return UPDATA_READY;
}

_NOINLINE_
u16 dev_update_by_change_stack(update_mode_info_t *p_info)
{
    /* log_info("2addr 0x%x\n", p_info); */
    /* put_buf((u8 *)p_info, sizeof(update_mode_info_t)); */

    /* 如果用户在切栈后需要对新的堆栈做保护，可以自行设置 */
    /* stack_change_step 2:已经切换成新的堆栈，打开堆栈保护，保护的区域由用户自行定义 */
    stack_protect_set(1, (u32)&d_new_stack_start[0], (u32)&d_new_stack_end[0]);
    update_mode_info_t info;
    memcpy((void *)&info, p_info, sizeof(update_mode_info_t));
    local_irq_enable();//第一次恢复

    /* stack_change_step 3:切换成新的堆栈后，用户需要运行的自定义函数 */
    app_active_update_task_init(&info);

    local_irq_disable();//第二次关
    /* stack_change_step 4:切栈后的自定义函数执行完成后，需要恢复原本的堆栈，恢复前需要关闭堆栈保护 */
    stack_protect_set(0, (u32)&_cpu0_sstack_begin, (u32)&_cpu0_sstack_end);
    return UPDATA_READY;

}

#endif



