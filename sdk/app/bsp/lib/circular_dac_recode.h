/***********************************Jieli tech************************************************
  File : .h
  By   : liujie
  Email: liujie@zh-jieli.com
  date : 2026-7-13
********************************************************************************************/
#ifndef __CIRCULAR_DAC_RECODE_H__
#define __CIRCULAR_DAC_RECODE_H__
#include "typedef.h"
#include "circular_buf.h"

void record_dac_init(void);
void record_dac(u8 *buf, u32 len);
u32 record_dac_read(cbuffer_t *user_buf);

/* u32 record_dac_len(void); */
u32 record_dac_len(u32 sr_need);


#endif


