#ifndef __ZBIT_LIB_AEC_H__
#define __ZBIT_LIB_AEC_H__
#include <stdint.h>


typedef enum {
    errZBIT_NoError = 0,
    errZBIT_NotReady,
    errZBIT_InvalidParam,
    errZBIT_InvalidLicense,
    errZBIT_BufferOverflow,
    errZBIT_BufferTooSmall,
} ZBIT_STATUS;

typedef struct {
    int32_t sample_rate;
    int32_t *shared_mem;
} zbit_init_pram_t;

uint8_t __attribute__((weak)) get_shared_mem_busy(void);

/// @brief Initialize the ECNR module
/// @param pram Pointer to initialization parameters
/// @return Status code
ZBIT_STATUS AI_ECNR_init(zbit_init_pram_t *pram);

/// @brief ECNR processing
/// @param audio Input/output audio buffer, 512 samples per processing frame (~10.66ms)
/// @param audio_ref Reference audio buffer
/// @return Status code
ZBIT_STATUS AI_ECNR_Processing(int16_t *audio, int16_t *audio_ref, int16_t *audio_out);

/// @brief Get ECNR version string
/// @return Pointer to version string
char *AI_ECNR_Version();

#endif // __ZBIT_LIB_AEC_H__
