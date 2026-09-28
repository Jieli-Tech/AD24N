/***********************************Jieli tech************************************************
  File : .c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2026-7-13
********************************************************************************************/
#include "config.h"
#include "app_config.h"
#include "audio_dac_cpu.h"
#include "circular_dac_recode.h"
#include "dac.h"

#include <string.h>
#include <stdint.h>

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[cir.dac.rec]"
#include "log.h"


typedef struct {
    u8  *rtpr;          // 录制起始指针
    u16 valid_bytes;    // 当前旁路有效字节
    u16 total_bytes;    // DAC环形总字节 sp_total * 2
} DAC_RECORD_CTL;

extern DAC_CTRL_HDL audio_dac_ops;
static DAC_RECORD_CTL g_dac_record_ctl = {0};
static volatile u16 recode_dac_inited;
void record_dac_init(void)
{
    memset((void *)(&g_dac_record_ctl), 0, sizeof(g_dac_record_ctl));
    g_dac_record_ctl.total_bytes = audio_dac_ops.sp_total * 2;
    recode_dac_inited = 0x55aa;
}

AT(.audio_d.text.cache.L2)
void record_dac(u8 *buf, u32 len)
{
    if (0x55aa != recode_dac_inited) {
        if (0x1234 == recode_dac_inited) {
            log_char('M');

        }
        return;
    }
    DAC_CTRL_HDL *hdl = &audio_dac_ops;
    DAC_RECORD_CTL *rec = &g_dac_record_ctl;
    u8 *buf_start = (u8 *)hdl->buf;
    u8 *buf_end = (u8 *)hdl->buf + hdl->sp_total * 2;
    /* u32 buf_total = hdl->sp_total * 2; */

    // 关中断保护全局状态读写
    local_irq_disable();
    {

        if (
            (((u32)rec->rtpr < (u32)buf_start) || ((u32)rec->rtpr >= (u32)buf_end))
            || (rec->valid_bytes == 0)
        ) {
            rec->rtpr = buf;
            rec->valid_bytes = len;
            local_irq_enable();
            return;
        } else {

            // 使用相对偏移量计算，避免直接指针比较
            u32 rec_end_offset = ((u32)rec->rtpr - (u32)buf_start + rec->valid_bytes) % rec->total_bytes;
            u32 buf_offset = ((u32)buf - (u32)buf_start) % rec->total_bytes;

            if (rec_end_offset != buf_offset) {
                log_char('N');
            }

        }

        rec->valid_bytes += len;

        // 溢出，丢弃最旧数据
        if (rec->valid_bytes > rec->total_bytes) {
            u32 overflow = rec->valid_bytes - rec->total_bytes;
            u32 rtpr_offset = (u32)rec->rtpr - (u32)buf_start;
            rtpr_offset = (rtpr_offset + overflow) % rec->total_bytes;
            rec->rtpr = buf_start + rtpr_offset;
            rec->valid_bytes = rec->total_bytes;
            log_char('y');
        }
        /* bit_set_swi(2); */
        /* extern bool HSuo_kick_aec_run(void *p_sound_out); */
        /* HSuo_kick_aec_run(NULL); */
    }
    local_irq_enable();
}

AT(.audio_a.text.cache.L2)
u32 record_dac_len(u32 sr_need)
{
    if (0x55aa != recode_dac_inited) {
        return 0;
    }

    /* u32 sr = dac_sr_read(); */
    /*
      *注意此处，如果程序会修改AUDIO DAC的采样率，那么此处不能用SR_DEFAULT;
     * 但也不能直接调用dac_sr_read，因为dac_sr_read放在flash中，而本函数放在RAM里面;会被AUDIO ADC 中断调用；
     * AUDIO ADC中断常驻在RAM中，在关中断临界操作时，多数时候不会关闭AUDIO ADC的中断。
     * */
    u32 sr =  SR_DEFAULT;
    u32 current_samples = g_dac_record_ctl.valid_bytes;
    // 参数合法性校验
    if (sr == 0) {
        return 0;
    }
    if (0 == sr_need) {
        return current_samples;
    }

    /* log_info("dac sr %d; adc sr %d",sr, sr_need); */
    // 直接使用 u32 运算（最大结果 = 768 * 192000 / 8000 ≈ 18432，远小于 u32 上限）
    u32 result = (current_samples * sr_need) / sr;

    return result;
}

u32 record_dac_read(cbuffer_t *user_buf)
{
    if (user_buf == NULL) {
        return 0;
    }
    recode_dac_inited = 0x1234;

    DAC_CTRL_HDL *hdl = &audio_dac_ops;
    DAC_RECORD_CTL *rec = &g_dac_record_ctl;
    u8 *buf_start = (u8 *)hdl->buf;
    u32 buf_end_addr;
    u8 *tmp_rtpr = NULL;
    u32 tmp_valid = 0;
    u32 tmp_total = 0;

    // 第一步：关中断，快照当前旁路状态，马上开中断
    local_irq_disable();
    tmp_rtpr  = rec->rtpr;
    tmp_valid = rec->valid_bytes;
    tmp_total = rec->total_bytes;
    /* local_irq_enable(); */

    if (tmp_rtpr == NULL || tmp_valid == 0 || tmp_total == 0) {
        local_irq_enable();
        recode_dac_inited = 0x55aa;
        return 0;
    }

    buf_end_addr = (u32)buf_start + tmp_total;
    u32 actual_read = tmp_valid;
    u32 remain_in_tail = buf_end_addr - (u32)tmp_rtpr;

    // 内存拷贝：无中断阻塞
    if (remain_in_tail >= actual_read) {
        actual_read = cbuf_write(user_buf, tmp_rtpr, actual_read);
    } else {
        u32 tmp2 = 0;
        u32 tmp1 = cbuf_write(user_buf, tmp_rtpr, remain_in_tail);
        if (remain_in_tail  == tmp1) {
            tmp2 = cbuf_write(user_buf, buf_start, actual_read - remain_in_tail);
        }
        actual_read = tmp1 + tmp2;
    }

    // 第二步：关中断更新读指针、有效长度
    /* local_irq_disable(); */
    {
        // 双重校验：防止读取期间写入侧溢出覆盖状态
        if (rec->valid_bytes >= actual_read) {
            u32 new_remain_tail = buf_end_addr - (u32)rec->rtpr;
            if (new_remain_tail >= actual_read) {
                rec->rtpr += actual_read;
            } else {
                rec->rtpr = buf_start + (actual_read - new_remain_tail);
            }
            rec->valid_bytes -= actual_read;
            if (rec->valid_bytes == 0) {
                rec->rtpr = NULL;
            }
        } else {
            // 读取中途被DAC写入溢出清空数据，本次读取作废
            log_char('H');
            actual_read = 0;
        }
    }
    local_irq_enable();

    recode_dac_inited = 0x55aa;

    return actual_read;
}


