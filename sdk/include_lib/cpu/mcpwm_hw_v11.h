#ifndef _MCPWM_HW_V11H_
#define _MCPWM_HW_V11H_

#define MCPWM_CH_MAX            2
#define MCPWM_TMR_BASE_ADDR     (&JL_MCPWM->TMR0_CON)
#define MCPWM_TMR_OFFSET        (&JL_MCPWM->TMR1_CON - &JL_MCPWM->TMR0_CON)
#define MCPWM_CH_BASE_ADDR      (&JL_MCPWM->CH0_CON0)
#define MCPWM_CH_OFFSET         (&JL_MCPWM->CH1_CON0 - &JL_MCPWM->CH0_CON0)
#define MCPWM_FPIN_BASE_ADDR (&JL_MCPWM->FPIN_CON)
#define MCPWM_FPIN_OFFSET         0
#define MCPWM_CON_BASE_ADDR     (&JL_MCPWM->MCPWM_CON0)
#define MCPWM_CON_OFFSET         0

//TMRx_CON reg
#define MCPWM_TMR_INCF  15
// #define MCPWM_TMR_RESERVE  14
#define MCPWM_TMR_UFPND 13
#define MCPWM_TMR_OFPN  12
#define MCPWM_TMR_UFCLR 11
#define MCPWM_TMR_OFCLR 10
#define MCPWM_TMR_UFIE  9
#define MCPWM_TMR_OFTE  8
#define MCPWM_TMR_CKSRC 7
#define MCPWM_TMR_CKPS  3 //4bit
#define MCPWM_TMR_CKPS_ 4 //4bit
// #define MCPWM_TMR_RESERVE  2
#define MCPWM_TMR_MODE  0 //2bit
#define MCPWM_TMR_MODE_ 2 //2bit

//TMRx_CNT reg

//TMRx_PRD reg

//CHx_CON0 reg
#define MCPWM_CH_DTCKPS     12 //4bit
#define MCPWM_CH_DTCKPS_    4 //4bit
#define MCPWM_CH_DTPR       7 //5bit
#define MCPWM_CH_DTPR_      5 //5bit
#define MCPWM_CH_DTEN       6
#define MCPWM_CH_L_INV      5
#define MCPWM_CH_H_INV      4
#define MCPWM_CH_L_EN       3
#define MCPWM_CH_H_EN       2
#define MCPWM_CH_CMP_LD     0 //2bit
#define MCPWM_CH_CMP_LD_    2 //2bit

//CHx_CON1 reg
#define MCPWM_CH_FPND   15
#define MCPWM_CH_FCLR   14
// #define MCPWM_CH_RESERVE   12 //2bit
#define MCPWM_CH_INTEN  11
#define MCPWM_CH_TMRSEL 8 //3bit
#define MCPWM_CH_TMRSEL_ 3 //3bit
// #define MCPWM_CH_reserve 5 //3bit
#define MCPWM_CH_FPINEN 4
#define MCPWM_CH_FPINAUTO   3
#define MCPWM_CH_FPINSEL    0 //3bit
#define MCPWM_CH_FPINSEL_   3 //3bit

//FPIN_CON reg
#define MCPWM_FPIN_EDGE  16 //8bit
#define MCPWM_FPIN_EDGE_  8 //8bit
#define MCPWM_FPIN_FLT_EN  8 //8bit
#define MCPWM_FPIN_FLT_EN_  8 //8bit
// #define MCPWM_CH_reserve 6 //2bit
#define MCPWM_FPIN_FLT_PR  8 //5bit
#define MCPWM_FPIN_FLT_PR_  5 //5bit

//MCPWM_CON reg
#define MCPWM_CON_CLK_EN    16
#define MCPWM_CON_TMR_EN    8 //8bit
#define MCPWM_CON_TMR_EN_    8 //8bit
#define MCPWM_CON_PWM_EN    0 //8bit
#define MCPWM_CON_PWM_EN_    8 //8bit


/* MCPWM TIMER寄存器 */
typedef struct _mcpwm_timer_reg {
    volatile u32 con;
    volatile u32 cnt;
    volatile u32 prd;
} MCPWM_TIMERx_REG;

/* MCPWM通道寄存器 */
typedef struct _mcpwm_ch_reg {
    volatile u32 con0;
    volatile u32 con1;
    volatile u32 cmph;
    volatile u32 cmpl;
} MCPWM_CHx_REG;

/* MCPWM FPIN寄存器 */
typedef struct _mcpwm_fpin_reg {
    volatile u32 con0;
} MCPWM_FPINx_REG;

/* MCPWM CON寄存器 */
typedef struct _mcpwm_con_reg {
    volatile u32 con0;
} MCPWM_CONx_REG;
#endif

