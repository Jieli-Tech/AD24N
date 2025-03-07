#ifndef _MCPWM_HW_H_
#define _MCPWM_HW_H_

#include "cpu.h"
#include "gpio.h"
#include "mcpwm_hw_v11.h"


#define MCPWM_TMR_PRD_SIZE    0xFFFF
#define MCPWM_TMR_PRH_SIZE    0xFFFF
#define MCPWM_TMR_PRL_SIZE    0xFFFF

/* pwm通道选择 */
typedef enum {
    MCPWM_CH0 = 0,
    MCPWM_CH1,
    MCPWM_CHx,
} mcpwm_ch;

#endif
