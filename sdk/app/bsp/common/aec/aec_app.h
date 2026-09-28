#ifndef __AEC_APP_H__
#define __AEC_APP_H__

#include "typedef.h"

/**
 * @brief 初始化麦克风及 AEC 算法（若启用）
 */
void aec_app_init(void);

void aec_app_process(void);

#endif
