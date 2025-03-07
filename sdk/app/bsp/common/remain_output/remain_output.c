
#include "remain_output.h"
#include "errno-base.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[norm]"
#include "log.h"


/* --------------------------------------------------------------------------*/
/**
 * @brief  初始化算法output相关信息
 *
 * @param p_ops     :
 * @param output    : output 函数
 * @param obuf      : output_buf
 * @param len       : output_buf长度
 *
 * @return  0成功，其他失败
 */
/* ----------------------------------------------------------------------------*/
u32 init_remain_obuf(remain_ops *p_ops, void *output, void *obuf, u32 len)
{
    if (NULL == p_ops) {
        return E_REMAIN_OPS_NULL;
    }
    memset(p_ops, 0, sizeof(remain_ops));
    p_ops->obuf = obuf;
    p_ops->len = len;
    p_ops->output = output;
    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  output输出
 *
 * @param priv      : priv参数
 * @param p_ops     :
 *
 * @return  输出长度
 */
/* ----------------------------------------------------------------------------*/
u32 remain_output(void *priv, remain_ops *p_ops)
{
    if (p_ops == NULL) {
        log_error("current output not init\n");
        return E_REMAIN_OPS_NULL;
    }

    if (0 == p_ops->remain_len) {     //no data output
        return 0;
    }
    /* log_info("remain %d\n",p_ops->remain_len); */
    /* log_info_hexdump((u8*)p_ops->obuf + p_ops->output_len ,p_ops->remain_len); */
    u32 olen = p_ops->output(priv, (u8 *)p_ops->obuf + p_ops->output_len, p_ops->remain_len);
    p_ops->remain_len -= olen;
    p_ops->output_len += olen;
    return olen;
}


/* --------------------------------------------------------------------------*/
/**
 * @brief  设置output需要输出数据长度
 *
 * @param p_ops     :
 * @param len       : output需要输出数据长度
 *
 * @return  0成功，其他失败
 */
/* ----------------------------------------------------------------------------*/
u32 set_remain_len(remain_ops *p_ops, u32 len)
{
    if (p_ops == NULL) {
        log_error("current output not init\n");
        return E_REMAIN_OPS_NULL;
    }

    if (0 != p_ops->remain_len) {     //当前outdata里仍有数据没输出
        return E_REMAIN_NOT_EMPTY;
    }

    if (len > p_ops->len) {
        log_error("set remain len > outdata_buf\n");
        return  E_REMAIN_OVER_SIZE;
    }

    p_ops->remain_len = len;
    p_ops->output_len = 0;
    return 0;
}





