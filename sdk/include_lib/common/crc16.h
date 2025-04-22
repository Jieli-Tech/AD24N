#ifndef __CPU_CRC16_H__
#define __CPU_CRC16_H__
#include "typedef.h"

// #define CRC16 	chip_crc16
extern u16 chip_crc16(const void *ptr, u32  len);   //CRC校验
u16 CRC16(const void *ptr, u32  len);

void CrcDecode(void  *buf, u16 len);
u16 get_appbin_crc16_value(void);
void set_crc16_reg_value(u16 value);
u16 get_crc16_reg_value();
u32 *get_crc16_fifo_ptr();
u16 CRC16_with_initval(void *ptr, u32  len, u32 init);

#endif


