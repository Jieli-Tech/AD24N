/**********************************************************
* API for KWS model library
* Copyright (C) 2022 Zbit Inc. All rights reserved.
**********************************************************/
#ifndef __LIB_KWS_H__
#define __LIB_KWS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum __KWS_STATUS {
    errKWS_NoError = 0,
    errKWS_NotReady,
    errKWS_InvalidParam,
    errKWS_InvalidLicense,
    errKWS_BufferOverflow,
    errKWS_BufferTooSmall
} KWS_STATUS;

/// @brief kws_init
/// @param buffer kws 公共区buffer
/// @param buf_size kws 公共区buffer size
/// @param buffer2 kws 私有区buffer
/// @param buffer2_size kws 私有区buffer size
/// @return
KWS_STATUS kws_init(int8_t *buffer, int32_t buf_size, int8_t *buffer2, int32_t buffer2_size);

/// @brief 加载音频
/// @param audio 16K采样率音频
/// @param nums 数量 128*2
/// @return
KWS_STATUS kws_load_input(int16_t *audio, int32_t audio_len);

/// @brief kws classify
/// @param top 识别词的ID
/// @param prob 置信度
/// @return
KWS_STATUS kws_classify(int16_t *top, int16_t *prob);

/// @brief reset beams
/// @param
/// @return
KWS_STATUS reset_beams(void);

/// @brief 获取kws 工作buffer大小
/// @param buf_size
/// @param buf_size2
/// @return
void getBufferSize(int32_t *buf_size, int32_t *buf_size2);

/// @brief set_kw_offset
/// @param kw_id:
/// @param func 0: offset >0 增强； < 0减弱，建议在-300 - 300之间，设置-10000就关闭了
/// @return
int8_t set_kw_offset(int16_t kw_id, int16_t offset);

/// @brief 版本号获取
/// @param
/// @return
char *kws_get_version(void);

#ifdef __cplusplus
}
#endif


#endif  // __LIB_KWS_H__
