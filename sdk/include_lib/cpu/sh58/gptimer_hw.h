#ifndef  __GPTIMER_HW_H__
#define  __GPTIMER_HW_H__

//BR56
#include "cpu.h"
#include "gpio.h"
#include "gptimer_hw_v5.h"

typedef JL_TIMER_TypeDef    GPTIMER;
#define GPTIMER0    JL_TIMER0
#define GPTIMER1    JL_TIMER1
#define GPTIMER2    JL_TIMER2
// #define GPTIMER3    JL_TIMER3
#define TIMER_MAX_NUM   3
#define TIMER_BASE_ADDR GPTIMER0
#define TIMER_OFFSET    (GPTIMER1 - GPTIMER0)

#define TIMER_CNT_SIZE  0xFFFF
#define TIMER_PRD_SIZE  0xFFFF
#define TIMER_PWM_SIZE  0xFFFF

#define IRQ_TIMEx_IDX_LIST  IRQ_TIME0_IDX, \
                            IRQ_TIME1_IDX, \
                            IRQ_TIME2_IDX, \
                            // IRQ_TIME3_IDX, \

#define IRFLT_SRC_TABLE_LIST    \
    0,  /* lsb_clk */ \
    0,  /* lsb_clk */ \
    12, /* std_12m */ \
    24, /* std_24m */ \

typedef enum gptimerx : u8 {
    TIMER0 = 0,
    TIMER1,
    TIMER2,
    // TIMER3,
    TIMERx, //传入此参数时,内部自动分配一个空闲TIMER
} timer_dev;


//以下宏定义给系统 timer 驱动使用
#define GPTIMER_PND_CLR         (0b1<<14)

#define GPTIMER_CLK_SRC_LSB     (0b0001<<10)
#define GPTIMER_CLK_SRC_RC250K  (0b0010<<10)
#define GPTIMER_CLK_SRC_RC16M   (0b0011<<10)
#define GPTIMER_CLK_SRC_LRC     (0b0100<<10)
#define GPTIMER_CLK_SRC_STD12M  (0b0101<<10)
#define GPTIMER_CLK_SRC_STD24M  (0b0110<<10)
#define GPTIMER_CLK_SRC_STD48M  (0b0111<<10)
#define GPTIMER_CLK_SRC_CLKOUT2 (0b1000<<10)
#define GPTIMER_CLK_SRC_PATCLK  (0b1001<<10)
#define GPTIMER_CLK_SRC_HSB     (0b1010<<10)
#define GPTIMER_CLK_SRC_WATCLK  (0b1011<<10)

#define GPTIMER_CLK_DIV_1       (0b0000<<4)
#define GPTIMER_CLK_DIV_4       (0b0001<<4)
#define GPTIMER_CLK_DIV_16      (0b0010<<4)
#define GPTIMER_CLK_DIV_64      (0b0011<<4)
#define GPTIMER_CLK_DIV_2       (0b0100<<4)
#define GPTIMER_CLK_DIV_8       (0b0101<<4)
#define GPTIMER_CLK_DIV_32      (0b0110<<4)
#define GPTIMER_CLK_DIV_128     (0b0111<<4)
#define GPTIMER_CLK_DIV_256     (0b1000<<4)
#define GPTIMER_CLK_DIV_1024    (0b1001<<4)
#define GPTIMER_CLK_DIV_4096    (0b1010<<4)
#define GPTIMER_CLK_DIV_16384   (0b1011<<4)
#define GPTIMER_CLK_DIV_512     (0b1100<<4)
#define GPTIMER_CLK_DIV_2048    (0b1101<<4)
#define GPTIMER_CLK_DIV_8192    (0b1110<<4)
#define GPTIMER_CLK_DIV_32768   (0b1111<<4)

#define GPTIMER_TIMER_MODE      (0b1<<0)

#define GPTIMER_SYS             GPTIMER2
#define GPTIMER_SYS_IRQ_INDEX   IRQ_TIME2_IDX
#define GPTIMER_SYS_START()     GPTIMER_SYS->CON |= GPTIMER_TIMER_MODE
#define GPTIMER_SYS_PAUSE()     GPTIMER_SYS->CON &= ~GPTIMER_TIMER_MODE
#define GPTIMER_SYS_EN_CHECK()  GPTIMER_SYS->CON & GPTIMER_TIMER_MODE
#define GPTIMER_SYS_CLR_PND()   GPTIMER_SYS->CON |= GPTIMER_PND_CLR

#define GPTIMER_SYS_INIT()      do{GPTIMER_SYS->CON = GPTIMER_PND_CLR|GPTIMER_CLK_SRC_STD12M|GPTIMER_CLK_DIV_4; \
                                GPTIMER_SYS->PRD = 0; \
                                GPTIMER_SYS->CNT = 0;}while(0) \

#define GPTIMER_SYS_GET_CNT     GPTIMER_SYS->CNT
#define GPTIMER_SYS_SET_CNT(x)  GPTIMER_SYS->CNT = (x)
#define GPTIMER_SYS_GET_PRD     GPTIMER_SYS->PRD
#define GPTIMER_SYS_SET_PRD(x)  GPTIMER_SYS->PRD = (x)-1
#define GPTIMER_SYS_CLK_SRC     12 //MHz单位
#define GPTIMER_SYS_CLK_DIV     2 // 右移两位 等效于 除以4

#endif


