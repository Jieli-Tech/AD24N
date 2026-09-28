#include "app_timer.h"
#include "log.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[tick2ms]"
#include "log.h"

static void (*p2ms_fun[P2MS_FUN_COUNT])(void) = {0};

/**
 * @brief 2ms调用一次
 *
 */
void tick_2ms_loop(void)
{
    // log_char('2');

    for (int i = 0; i < P2MS_FUN_COUNT; i++) {
        if (p2ms_fun[i]) {
            p2ms_fun[i]();
        }
    }

    return;
}


inline void regist_2ms_fun(uint8_t index, void(*fun)(void))
{
    log_info("\r\n app fun index: %d\r\n fun address: %p\r\n", index, fun);
    p2ms_fun[index] = fun;

    return;
}

inline void unregist_2ms_fun(uint8_t index)
{
    p2ms_fun[index] = NULL;

    return;
}
