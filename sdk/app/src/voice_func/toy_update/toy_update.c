/***********************************Jieli tech************************************************
  File : toy_update.c
  By   : mawancheng
  date : 2025-4-11
  brief: 该文件为独立的升级应用，公共消息check完ufw文件后会跳转到该应用，该应用会使用ld文件的d_u         pdate_and_new_stack代码段占用的ram，不可以和其他overlay相关的代码一起运行
********************************************************************************************/
#include "toy_update.h"
#include "toy_main.h"
#include "includes.h"
#include "msg.h"
#include "key.h"
#include "bsp_loop.h"
#include "device_mge.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[update_app]"
#include "log.h"


void toy_update_app(void)
{
    u32 err;
    int msg[2];

    log_info("toy_update_mode!\n");


    /* err = get_msg(4, &msg[0]); */
    /* if (MSG_NO_ERROR != err) { */
    /* msg[0] = NO_MSG; */
    /* log_info("get msg err 0x%x\n", err); */
    /* } */

    /* log_info("msg[0] 0x%x\n", msg[0]); */
    /* log_info("msg[1] 0x%x\n", msg[1]); */
    /* u8 dev_idx = msg[1] - MSG_USB_DISK_IN; */
    err = device_update(NULL, 0);

    if (err) {
        /* 升级失败，切到下一个模式 */
        log_error("toy_update_err 0x%x\n", err);
        post_msg(1, MSG_NEXT_WORKMODE);
    }
    while (1) {
        err = get_msg(2, &msg[0]);
        if (MSG_NO_ERROR != err) {
            msg[0] = NO_MSG;
            log_info("get msg err 0x%x\n", err);
        }
        bsp_loop();

        if (NO_MSG == msg[0]) {
            continue;
        }

        switch (msg[0]) {
        case MSG_CHANGE_WORK_MODE:
            goto __toy_update_exit;
        default:
            common_msg_deal(&msg[0]);
            break;
        }
    }
__toy_update_exit:
    return;
}
