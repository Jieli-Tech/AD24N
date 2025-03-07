#include "typedef.h"
#include "pl_cnt.h"
#include "gpio_hw.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[pl_cnt]"
#include "log.h"

static const struct pl_cnt_platform_data *__this = NULL;
static JL_PORT_TypeDef *PL_CNT_IO_PORTx = NULL;
static u8 PL_CNT_IO_xx;

/*----------------------------------------------------------------------------*/
/**@brief   设置对应IO进入捕获通道
   @param   port: 对应IO，如IO_PORTA_10
   @return  0：成功；
            -1：设置IO超过最大IO数
   @author  liuhaokun
   @note    static u32 pl_cnt_iomc(u8 port)
*/
/*----------------------------------------------------------------------------*/
static u32 pl_cnt_iomc(u8 port)
{
    u8 input_start = 0;
    if (port > IO_MAX_NUM) {
        return -1;
    } else if ((port / 16) == 0) {
        input_start = PA0_IN;
        PL_CNT_IO_PORTx = JL_PORTA;
    } else if ((port / 16) == 1) {
        input_start = PB0_IN;
        PL_CNT_IO_PORTx = JL_PORTB;
    }
    PL_CNT_IO_xx = port % 16;
    gpio_ich_sel_input_signal(port, ICH_CAP, ICH_TYPE_GP_ICH);

    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   触摸驱动初始化函数
   @param   pdata:  触摸驱动句柄
   @return  无
   @author  liuhaokun
   @note    void pl_cnt_init(const struct pl_cnt_platform_data *pdata)
*/
/*----------------------------------------------------------------------------*/
void pl_cnt_init(const struct pl_cnt_platform_data *pdata)
{
    __this = pdata;
    if (NULL == __this) {
        log_error("pl_cnt_init pdata is null\n");
        return;
    }
    u32 ret = 0;
    for (u8 i = 0; i < pdata->port_num; i ++) {
        ret = pl_cnt_iomc(pdata->port[i]);
        if (0 != ret) {
            log_error("current io %d is not legal\n", pdata->port[i]);
            continue;
        }
        //引脚先给寄生电容充电
        PL_CNT_IO_PORTx->DIE |=  BIT(PL_CNT_IO_xx);
        PL_CNT_IO_PORTx->OUT |=  BIT(PL_CNT_IO_xx);
        PL_CNT_IO_PORTx->DIR &= ~BIT(PL_CNT_IO_xx);
        //放电时，关上拉，开下拉
        PL_CNT_IO_PORTx->PU0 &= ~BIT(PL_CNT_IO_xx);
        PL_CNT_IO_PORTx->PD0 |=  BIT(PL_CNT_IO_xx);
    }
    JL_PCNT->CON = 0;
    JL_PCNT->CON |= (0b11 << 2);      //选择d1p0为时钟源
    JL_PCNT->CON |= BIT(1);           //引脚放电计数使能
}



extern void delay(volatile u32 t);
/*----------------------------------------------------------------------------*/
/**@brief   获取触摸按键对应通道采集值
   @param   ch: 通道
   @return  0：成功；
            sum_val：对应通道的IO的触摸采集值
   @author  liuhaokun
   @note    u32 get_pl_cnt_value(u8 ch)
*/
/*----------------------------------------------------------------------------*/
u32 get_pl_cnt_value(u8 ch)
{
    if (NULL == __this) {
        log_error("get_pl_cnt_value pdata is null\n");
        return 0;
    }
    pl_cnt_iomc(__this->port[ch]);
    u32 tmp_val, start_val, end_val, sum_val = 0;
    for (u8 i = 0; i < __this->sum_num; i ++) {
        delay(__this->charge_time);                     //确保充满电
        start_val = JL_PCNT->VAL;                       //保存开始计数值
        PL_CNT_IO_PORTx->DIR |= BIT(PL_CNT_IO_xx);      //切换方向，下拉电阻起作用，此时开始放电，放电计数模块开始计数
        delay(__this->charge_time);
        /* while (PL_CNT_IO_PORTx->IN & BIT(PL_CNT_IO_xx));//等待放电结束，直到读到0 */
        end_val = JL_PCNT->VAL;                         //保存结束计数值
        PL_CNT_IO_PORTx->DIR &= ~BIT(PL_CNT_IO_xx);     //切换方向，输出1，即又开始对寄生电容充电
        if (end_val > start_val) {
            tmp_val = end_val - start_val;
        } else {
            tmp_val = (u32) - start_val + end_val;
        }
        sum_val += tmp_val;
    }
    sum_val = sum_val / __this->sum_num;

    /* 用户可打开下方打印，查看对应IO口在触摸与正常时的计数值为多少从而设置TOUCH_DELDA阈值 */
    /* log_info("%d %d\n",__this->port[ch], sum_val); */

    gpio_ich_disable_input_signal(__this->port[ch], ICH_CAP, ICH_TYPE_GP_ICH);
    return sum_val;
}
