/***********************************Jieli tech************************************************
  File : seq_buf.h
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2026-7-10
********************************************************************************************/
#ifndef __SEQ_BUF_H__
#define __SEQ_BUF_H__
#include "typedef.h"

/* ===================== 1. 顺序缓冲区结构体 ===================== */
/**
 * @brief 顺序累加缓冲区（Linear Sequential Accumulation Buffer）
 *
 * 用于将数据连续追加到尾部，需外部调用者负责在数据发送后重置 data_len。
 * 不支持环形覆盖，适用于“攒包发送”场景。
 */
typedef struct _SEQ_BUF {
    u32 data_len;   /**< 当前已累积的有效字节数（从 buff 起始偏移） */
    u32 buff_len;   /**< 缓冲区总容量（字节），必须与分配的内存大小一致 */
    u8  *buff;      /**< 指向外部分配的线性内存区域，长度至少为 buff_len */
} SEQ_BUF;


u32 seq_buf_init(SEQ_BUF *p_seq_buf, u8 *buff, u32 buff_len);
u32 seq_buf_write(SEQ_BUF *p_seq_buf, const u8 *ibuff, u32 ibuff_len);
u32 seq_data_used(SEQ_BUF *p_seq_buf, u32 used_len);
u32 seq_buf_expand(SEQ_BUF *p_seq_buf, u32 target_len);

/*
u32 seq_buf_init(SEQ_BUF *p_seq_buf, u8 *buff, u32 buff_len)
{
	if (p_seq_buf == NULL) {
		log_error("seq_buf init pointer is NULL");
		return 0;
	}
    p_seq_buf->data_len = 0;
    p_seq_buf->buff_len = buff_len;
    p_seq_buf->buff = buff
    return (u32)p_seq_buf->buff;
}

u32 seq_buf_write(SEQ_BUF *p_seq_buf, const u8 *ibuff, u32 ibuff_len)
{
    // ------ 参数有效性检查 ------
    if (p_seq_buf == NULL) {
        log_error("seq_buf pointer is NULL");
        return 0;
    }
    if (p_seq_buf->buff == NULL) {
        log_error("seq_buf buff is NULL");
        return 0;
    }
    if (ibuff == NULL) {
        log_error("seq input buffer is NULL");
        return 0;
    }
    if (ibuff_len == 0) {
        log_error("seq input length is zero");
        return 0;
    }

    // 修复可能的数据不一致（保护性截断）
    if (p_seq_buf->data_len > p_seq_buf->buff_len) {
        p_seq_buf->data_len = p_seq_buf->buff_len;
    }

    u32 cnt = 0;          // 已写入字节数
    u32 total = p_seq_buf->buff_len;
    u32 used  = p_seq_buf->data_len;

    // 循环拷贝，每次取可用空间与剩余输入中的较小值
    while (cnt < ibuff_len && used < total) {
        u32 free_space = total - used;        // 剩余可用空间
        u32 remain = ibuff_len - cnt;         // 剩余待写数据
        u32 chunk = (free_space < remain) ? free_space : remain;

        memcpy(p_seq_buf->buff + used, ibuff + cnt, chunk);
        used += chunk;
        cnt += chunk;
    }

    p_seq_buf->data_len = used;   // 更新已用长度
    return cnt;                    // 返回实际写入字节数
}

u32 seq_data_used(SEQ_BUF *p_seq_buf, u32 used_len)
{
    if (p_seq_buf == NULL || used_len == 0) {
        return 0;
    }

    if (used_len >= p_seq_buf->data_len) {
        p_seq_buf->data_len = 0;
        return used_len;
    }

    u32 remain_len = p_seq_buf->data_len - used_len;

    if (remain_len > 0 && p_seq_buf->buff != NULL) {
        memmove(p_seq_buf->buff, p_seq_buf->buff + used_len, remain_len);
        p_seq_buf->data_len = remain_len;
    }

    return used_len;
}



*/

#endif





