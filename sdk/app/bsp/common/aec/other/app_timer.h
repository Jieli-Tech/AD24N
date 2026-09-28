#ifndef __APP_TIMER_H__
#define __APP_TIMER_H__

#include <stdint.h>
#include <string.h>


#define P2MS_FUN_COUNT     2


#define KWS_REGIST_FUN_INDEX                 0
#define KWS_SLEEP_FUN_INDEX                  1

//void app_timer_clear(uint8_t timer);
//void app_timer_regist(uint8_t timer, uint32_t ms2, void (*fun)(void));
//void app_timer_unregist(uint8_t timer);

void regist_2ms_fun(uint8_t index, void(*fun)(void));
void unregist_2ms_fun(uint8_t index);

#endif
