#ifndef __KEY_DRV_IR_H__
#define __KEY_DRV_IR_H__


enum timer_sel {
    SEL_TIMER0 = 0x0,
    SEL_TIMER1 = 0x1,
    SEL_TIMER2 = 0x2,
};

typedef struct _IR_CODE {
    u16 wData;          //<
    u8  bState;         //<
    u16 wUserCode;      //<
    u8  boverflow;      //<
} IR_CODE;


struct irflt_platform_data {
    u8 irflt_io;
    u8 timer;
};


void ir_timeout(void);
u8 get_irkey_value(void);
int irflt_init(void *arg);

#endif

