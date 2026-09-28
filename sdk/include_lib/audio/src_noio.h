/***********************************Jieli tech************************************************
  File : src_noio.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2026-7-10
********************************************************************************************/
#ifndef __SRC_NOIO_H__
#define __SRC_NOIO_H__
#include "string.h"
#include "src.h"
#include "config.h"
#include "seq_buf.h"


/* ===================== 2. 重采样上下文（无 io 回调） ===================== */
/**
 * @brief 重采样处理上下文，去掉了 io 成员，改用外部 SEQ_BUF 输出
 *
 * 保留了原有的参数、操作接口、信息以及内部暂存区。
 * rmlen/rmcnt 用于处理上次未完全输出的残余数据。
 */
typedef struct _SRC_BUFF_NOIO {
    SRC_PARA_STRUCT para;   /**< 重采样参数（如最大输入块大小等） */
    SRC_INFO        info;   /**< 重采样状态信息（如输出样点数等） */
    u8              buff[SRC_MAX_BUFF * 2]; /**< 内部暂存区，用于存放一帧输出数据 */
    u16             rmlen;  /**< 残余数据长度（字节） */
    u16             rmcnt;  /**< 残余数据中已发送的偏移量（字节） */
} SRC_BUFF_NOIO;

int src_run_noio(SRC_BUFF_NOIO *p_src_noio, SEQ_BUF *p_seq_obuf, short *inbuf, int len);
int src_noio_open(SRC_BUFF_NOIO *p_src_noio, int in_sr, int out_sr);

#endif
