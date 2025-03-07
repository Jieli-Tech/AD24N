#ifndef _MCPWM_H_
#define _MCPWM_H_

#include "mcpwm_hw.h"

#define MCPWM_NUM_MAX     MCPWM_CH_MAX


enum mcpwm_err : u8 { //返回值类型
    MCPWM_ERR_INIT_FAIL = 0xFF,
};
enum mcpwm_para : u8 { //从寄存器当前状态获取timer参数
    MCPWM_PARA_MODE = 0,
    MCPWM_PARA_FREQ,
    MCPWM_PARA_DUTY,
    MCPWM_PARA_H_DUTY,
    MCPWM_PARA_H_PIN,
    MCPWM_PARA_L_DUTY,
    MCPWM_PARA_L_PIN,
    MCPWM_PARA_DET_PIN,
    MCPWM_PARA_EDGE,
    MCPWM_PARA_IRQ_CALLBACK,
    MCPWM_PARA_IRQ_PRI,
};


/* 对齐方式选择 */
typedef enum {
    MCPWM_EDGE_ALIGNED = 1,  ///< 边沿对齐模式
    MCPWM_CENTER_ALIGNED, ///< 中心对齐模式
} mcpwm_aligned_mode;

/* 故障保护触发边沿 */
typedef enum {
    MCPWM_EDGE_FAILL = 0, //下降沿触发
    MCPWM_EDGE_RISE,  //上升沿触发
    MCPWM_EDGE_DEFAULT = 0xff, //默认会忽略
} mcpwm_edge;

typedef void (*mcpwm_detect_irq_cb)(u32 ch); //回调函数

struct mcpwm_config {
    mcpwm_ch ch; //通道号
    mcpwm_aligned_mode mode;
    u32 freq;
    u16 h_duty;
    u16 h_pin;
    u16 l_duty;
    u16 l_pin;
    u16 detect_port;
    mcpwm_edge edge;
    mcpwm_detect_irq_cb irq_cb;
    u8 irq_priority;
    u16 dead_time;
};

//以下为对外api接口
u32 mcpwm_init(const struct mcpwm_config *cfg);
u32 mcpwm_deinit(u32 id);
u32 mcpwm_start(u32 id);
u32 mcpwm_pause(u32 id);
u32 mcpwm_resume(u32 id);
u32 mcpwm_set_freq(u32 id, u32 freq);
u32 mcpwm_set_duty(u32 id, u32 h_duty, u32 l_duty);
u32 mcpwm_set_irq_callback(u32 id, u32 priority, mcpwm_detect_irq_cb irq_cb);
u32 mcpwm_info_dump(u32 id);




//以下为底层驱动接口, 直接操作寄存器
// u32 mcpwm_hw_init(u32 ch, u32 freq, u16 h_duty, u16 h_pin, u16 l_duty, u16 l_pin);
u32 mcpwm_hw_init(u32 ch);
u32 mcpwm_hw_deinit(u32 ch);
u32 mcpwm_hw_enable(u32 ch);
u32 mcpwm_hw_disable(u32 ch);
u32 mcpwm_hw_set_freq_duty(u32 ch, u32 freq, mcpwm_aligned_mode mode, u32 h_duty, u32 l_duty);
u32 mcpwm_hw_set_port(u32 ch, u32 h_pin, u32 l_pin);
u32 mcpwm_hw_set_detect_port(u32 ch, u32 port, mcpwm_edge edge);
u32 mcpwm_hw_set_irq_callback(u32 ch, u32 priority, mcpwm_detect_irq_cb irq_cb);
u32 mcpwm_hw_set_dead_time(u32 ch, u32 ns);
u32 mcpwm_hw_detect_pnd_clr(u32 ch);
u32 mcpwm_reg_dump(u32 ch);


#endif
