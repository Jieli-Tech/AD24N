/***********************************Jieli tech************************************************
  File : src_to_buffer.h
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2026-7-10
********************************************************************************************/
#ifndef __SRC_TO_BUFFER__
#define __SRC_TO_BUFFER__

#include "src_noio.h"
#include "circular_buf.h"
void *src_noio_reless(SRC_BUFF_NOIO *p_src_noio);
SRC_BUFF_NOIO *src_noio_api(u32 insample, u32 outsample);
int src_run_noio_cbuf(SRC_BUFF_NOIO *p_src_noio, SEQ_BUF *p_seq_obuf, cbuffer_t *p_cbuf);

#endif
