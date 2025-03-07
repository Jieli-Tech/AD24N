#ifndef __KEY_DRV_IO_H__
#define __KEY_DRV_IO_H__

#include "gpio.h"
#include "key.h"

#define IS_KEY0_DOWN()    	(!(JL_PORTA->IN & BIT(10)))
#define IS_KEY1_DOWN()    	(!(JL_PORTA->IN & BIT(11)))
#define IS_KEY2_DOWN()    	(!(JL_PORTA->IN & BIT(12)))

#define KEY_INIT()        do{\
                            /**key0 init*/\
                            JL_PORTA->PU0 |= BIT(10),\
                            JL_PORTA->PD0 &= ~BIT(10),\
                            JL_PORTA->DIE |= BIT(10),\
                            JL_PORTA->DIR |= BIT(10);\
                            /**key1 init*/\
                            JL_PORTA->PU0 |= BIT(11),\
                            JL_PORTA->PD0 &= ~BIT(11),\
                            JL_PORTA->DIE |= BIT(11),\
                            JL_PORTA->DIR |= BIT(11);\
                            /**key2 init*/\
                            JL_PORTA->PU0 |= BIT(12),\
                            JL_PORTA->PD0 &= ~BIT(12),\
                            JL_PORTA->DIE |= BIT(12),\
                            JL_PORTA->DIR |= BIT(12);\
                            }while(0)

extern const key_interface_t key_io_info;

#endif/*__KEY_DRV_IO_H__*/
