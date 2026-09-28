/***********************************Jieli tech************************************************
  File : src_to_buffer.c
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2026-7-10
********************************************************************************************/
#include "typedef.h"
#include "config.h"
#include "my_malloc.h"
#include "src_to_buffer.h"
#include "circular_buf.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[src2buf]"
#include "log.h"



void *src_noio_reless(SRC_BUFF_NOIO *p_src_noio)
{
    my_free(p_src_noio);
    return NULL;
}


SRC_BUFF_NOIO *src_noio_api(u32 insample, u32 outsample)
{
    SRC_BUFF_NOIO *p_src_noio = my_malloc(sizeof(SRC_BUFF_NOIO), MM_HW_SRC_NOIO);
    if (NULL == p_src_noio) {
        log_error("SRC_NOIO MALLOC ERR!!!\n");
        return NULL;
    }
    u32 err = src_noio_open(p_src_noio, insample, outsample);
    if (0 != err) {
        log_error("SRC_NOIO OPEN ERR!!!\n");
        my_free(p_src_noio);
        return NULL;
    }
    return p_src_noio;
}

/* int src_run_noio(SRC_BUFF_NOIO *p_src_noio, SEQ_BUF *p_seq_obuf, short *inbuf, int len) */

int src_run_noio_cbuf(SRC_BUFF_NOIO *p_src_noio, SEQ_BUF *p_seq_obuf, cbuffer_t *p_cbuf)
{

    s16 *rptr;
    u32 olen, tlen;
    while (1) {
        // log_char('e');
        rptr = cbuf_read_alloc(p_cbuf, &olen);
        // log_info("src_run_noio_cbuf: 0x%x,0x%x,0x%x", (u32)p_src_noio, (u32)rptr, (u32)olen);
        tlen = src_run_noio(p_src_noio, p_seq_obuf, rptr, olen);
        cbuf_read_updata(p_cbuf, tlen);
        // log_info(" %d,%d", tlen, olen);
        if (tlen != olen) {
            break;
        } else if (0 == olen) {
            break;
        }
    }
    // log_info("src_run_noio_cbuf over: 0x%x,0x%x,0x%x", (u32)p_src_noio, (u32)rptr, (u32)olen);
    return  p_seq_obuf->data_len;
}

