#include "key_ir.h"
#include "ir_decoder.h"
#include "gptimer.h"
#include "app_config.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"


#if KEY_IR_EN

#if (STANDARD_IR == SEL_IR_MODE)
/* 标准ENC协议红外按键驱动自动选择空闲timer使用 */
static struct gptimer_config if_dec_cfg;
#else
/*  简易红外按键驱动选择IR_KEY_TIMER使用 */
static const struct irflt_platform_data irflt_data = {
    .irflt_io       = IR_KEY_IO_SEL,
    .timer          = IR_KEY_TIMER,
};
#endif

static const u8 IRTabFF00[] = {
    NKEY_00, NKEY_01, NKEY_02, NKEY_03, NKEY_04, NKEY_05, NKEY_06, IR_06, IR_15, IR_08, NKEY_0A, NKEY_0B, IR_12, IR_11, NKEY_0E, NKEY_0F,
    NKEY_10, NKEY_11, NKEY_12, NKEY_13, NKEY_14, IR_07, IR_09, NKEY_17, IR_13, IR_10, NKEY_1A, NKEY_1B, IR_16, NKEY_1D, NKEY_1E, NKEY_1F,
    NKEY_20, NKEY_21, NKEY_22, NKEY_23, NKEY_24, NKEY_25, NKEY_26, NKEY_27, NKEY_28, NKEY_29, NKEY_2A, NKEY_2B, NKEY_2C, NKEY_2D, NKEY_2E, NKEY_2F,
    NKEY_30, NKEY_31, NKEY_32, NKEY_33, NKEY_34, NKEY_35, NKEY_36, NKEY_37, NKEY_38, NKEY_39, NKEY_3A, NKEY_3B, NKEY_3C, NKEY_3D, NKEY_3E, NKEY_3F,
    IR_04, NKEY_41, IR_18, IR_05, IR_03, IR_00, IR_01, IR_02, NKEY_48, NKEY_49, IR_20, NKEY_4B, NKEY_4C, NKEY_4D, NKEY_4E, NKEY_4F,
    NKEY_50, NKEY_51, IR_19, NKEY_53, NKEY_54, NKEY_55, NKEY_56, NKEY_57, NKEY_58, NKEY_59, IR_17, NKEY_5B, NKEY_5C, NKEY_5D, IR_14, NKEY_5F,
};

/*----------------------------------------------------------------------------*/
/**@brief   按键去抖函数，输出稳定键值
   @param   key：键值
   @return  稳定按键
   @note    u8 key_filter(u8 key)
*/
/*----------------------------------------------------------------------------*/
static u8 key_filter(u8 key)
{
    static u8 used_key = NO_KEY;
    static u8 old_key;
    static u8 key_counter;

    if (old_key != key) {
        key_counter = 0;
        old_key = key;
    } else {
        key_counter++;
        if (key_counter == KEY_BASE_CNT) {
            used_key = key;
        }
    }
    return used_key;
}


/*----------------------------------------------------------------------------*/
/**@brief   ir按键初始化
   @param   void
   @param   void
   @return  void
   @note    void ir_key_init(void)
*/
/*----------------------------------------------------------------------------*/
static void ir_key_init(void)
{
#if (STANDARD_IR == SEL_IR_MODE)
    log_info("standard ir key init\n");
    memset((void *)&if_dec_cfg, 0, sizeof(struct gptimer_config));
    if_dec_cfg.mode = GPTIMER_MODE_CAPTURE_EDGE_FALL;
    if_dec_cfg.capture.port = (IR_KEY_IO_SEL / 16);
    if_dec_cfg.capture.pin = BIT(IR_KEY_IO_SEL % 16);
    if_dec_cfg.capture.filter = 0;
    if_dec_cfg.capture.max_period = 110 * 1000;
    if_dec_cfg.irq_cb = NULL;   //NULL即为采用标准NEC协议,若用户自定义红外协议，自行编写该中断即可
    if_dec_cfg.irq_priority = IRQ_IRTMR_IP,
               ir_decoder_init(&if_dec_cfg);
#else
    irflt_init((void *)&irflt_data);
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief   获取IR按键电平值
   @param   void
   @param   void
   @return  key_num:ir按键号
   @note    u8 irkey_get_value(void)
*/
/*----------------------------------------------------------------------------*/
static u32 irkey_get_value(void)
{
    u8 key_num = NO_KEY;
#if (STANDARD_IR == SEL_IR_MODE)
    u8 ir_value = get_new_irkey_value();
#else
    u8 ir_value = get_irkey_value();
#endif
    if (ir_value != NO_KEY) {
        key_num = IRTabFF00[ir_value];
    }
    return key_num;
}

const key_interface_t key_ir_info = {
    .key_type = KEY_TYPE_IR,
    .key_init = ir_key_init,
    .key_get_value = irkey_get_value,
};

#endif/*KEY_IR_EN*/
