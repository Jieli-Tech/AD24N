
#include "typedef.h"
#include "irflt.h"
#include "gpio.h"
#include "app_config.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[irflt]"
#include "log.h"

/* 该源文件驱动为简易红外按键驱动使用 */
static JL_TIMER_TypeDef *IRTMR = JL_TIMER2;
static u8 IRQ_IRTMR = IRQ_TIME2_IDX;

static IR_CODE  ir_code;       ///<红外遥控信息
static u16 irtmr_prd;
volatile u8 ir_busy = 0;

/*----------------------------------------------------------------------------*/
/**@brief   简易红外busy超时函数
   @param   void
   @param   void
   @return  void
   @note    void ir_timeout(void)
*/
/*----------------------------------------------------------------------------*/
void ir_timeout(void)
{
#if (SIMPLE_IR == SEL_IR_MODE)
    ir_code.boverflow++;
    if (ir_code.boverflow > 10) { //120ms
        ir_code.boverflow = 10;
        ir_code.bState = 0;
        ir_busy = 0;
    }
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief   time红外中断服务函数
   @param   void
   @param   void
   @return  void
   @note    void timer1_ir_isr(void)
*/
/*----------------------------------------------------------------------------*/
___interrupt
void irtmr_ir_isr(void)
{
    static u8 cnt = 0;
    IRTMR->CON |= BIT(14);
    IRTMR->CNT = 0;

    u16 bCap1 = IRTMR->PRD;
    u8 cap = bCap1 / irtmr_prd;

    /* log_info("ir isr verify"); */
    if (cap <= 1) {
        ir_code.wData >>= 1;
        ir_code.bState++;
        ir_code.boverflow = 0;
    } else if (cap == 2) {
        ir_code.wData >>= 1;
        ir_code.bState++;
        ir_code.wData |= 0x8000;
        ir_code.boverflow = 0;
    }
    /*13ms-Sync*/
    else if ((cap == 13) && (ir_code.boverflow < 8)) {
        ir_code.bState = 0;
        ir_busy = 1;
    } else if ((cap < 8) && (ir_code.boverflow < 5)) {
        ir_code.bState = 0;
    } else if ((cap > 110) && (ir_code.boverflow > 53)) {
        ir_code.bState = 0;
    } else if ((cap > 20) && (ir_code.boverflow > 53)) { //溢出情况下 （12M 48M）
        ir_code.bState = 0;
    } else {
        ir_code.boverflow = 0;
    }

    if (ir_code.bState == 16) {
        /* ir_code.wUserCode = ir_code.wData; */
    }
    if (ir_code.bState == 32) {
        /* log_info("[0x%X]\n", ir_code.wData); */
    }
    /* log_info("cnt %d; ir_code.wData 0x%x\n",cnt++, ir_code.wData); */
}

/*----------------------------------------------------------------------------*/
/**@brief   红外按键输入IO设置函数
   @param   void
   @param   void
   @return  void
   @note    void ir_input_io_sel(u8 port)
*/
/*----------------------------------------------------------------------------*/
static void ir_input_io_sel(u8 port)
{
    gpio_set_mode(IO_PORT_SPILT(port), PORT_INPUT_FLOATING);
    gpio_ich_sel_input_signal(port, ICH_TIMER0_CAPTURE + (IRQ_IRTMR - IRQ_TIME0_IDX), ICH_TYPE_GP_ICH);
}



/*----------------------------------------------------------------------------*/
/**@brief   红外按键相关寄存器设置函数
   @param   void
   @param   void
   @return  void
   @note    void irflt_config(void)
*/
/*----------------------------------------------------------------------------*/
static void irflt_config(void)
{
    u32 clk;
    u32 prd_cnt;

    IRTMR->IRFLT = 0;
    SFR(IRTMR->IRFLT, 4, 4, 9);//512 div
    SFR(IRTMR->IRFLT, 2, 2, 3);//std24M
    /* 则 512 / 24M = 21.3us，小于该值的窄冲脉信号会被滤掉 */
    SFR(IRTMR->IRFLT, 0, 1, 1);//下降沿捕获

    IRTMR->CON = BIT(14);
    IRTMR->PRD = 0;
    IRTMR->CNT = 0;

    /* 设置timer的时钟源，需注意最终时钟需要在(lsb / 2)以下 */
    SFR(IRTMR->CON, 10, 4, 5); //std12M (lsb/2以下)
    SFR(IRTMR->CON, 4, 4, 3); //pset=8,64分频

    clk = 12000000;//
    clk /= (1000 * 64);//1ms for cnt
    prd_cnt = clk;
    irtmr_prd = prd_cnt;//irtmr_prd = 187us

    request_irq(IRQ_IRTMR, IRQ_IRTMR_IP, irtmr_ir_isr, 0);
    SFR(IRTMR->CON, 16, 1, 0); //2 edge dis
    SFR(IRTMR->CON, 2, 2, 1); //irflt en
    SFR(IRTMR->CON, 0, 2, 3); //mode falling edge
}

/*----------------------------------------------------------------------------*/
/**@brief   获取ir按键值
   @param   void
   @param   void
   @return  void
   @note    u8 get_irkey_value(void)
*/
/*----------------------------------------------------------------------------*/
u8 get_irkey_value(void)
{
    u8 tkey = 0xff;
    if (ir_code.bState != 32) {
        return tkey;
    }
    if ((((u8 *)&ir_code.wData)[0] ^ ((u8 *)&ir_code.wData)[1]) == 0xff) {
        /* if (ir_code.wUserCode == 0xFF00) */
        {
            /* log_info("<%d>",(u8)ir_code.wData); */
            tkey = (u8)ir_code.wData;
        }
    } else {
        ir_code.bState = 0;
    }
    return tkey;
}

/*----------------------------------------------------------------------------*/
/**@brief   ir按键初始化
   @param   void
   @param   void
   @return  void
   @note    void ir_key_init(void)
*/
/*----------------------------------------------------------------------------*/
int irflt_init(void *arg)
{
    log_info("irflt_init >>>\n");
    struct irflt_platform_data *user_data = (struct irflt_platform_data *)arg;

    /* 1.选择红外timer源 */
    if (user_data->timer == SEL_TIMER0) {
        IRTMR = JL_TIMER0;
        IRQ_IRTMR = IRQ_TIME0_IDX;
    } else if (user_data->timer == SEL_TIMER1) {
        IRTMR = JL_TIMER1;
        IRQ_IRTMR = IRQ_TIME1_IDX;
    } else {
        IRTMR = JL_TIMER2;
        IRQ_IRTMR = IRQ_TIME2_IDX;
    }

    /* 2.红外IO设置 */
    ir_input_io_sel(user_data->irflt_io);

    /* 3.红外模块寄存器设置 */
    irflt_config();

    return 0;
}

