#include "lib_kws.h"

#include "log.h"
#include "gpio.h"
#include "config.h"
#include "kws_task.h"
#include "kws_handler.h"
#include "dac_api.h"
#include "app_timer.h"
#include "msg.h"

#include "libkws_AD24N_2001_180k_V3.1.0_cn_wd5_LimitedTime_bfdbbe7_20260907_193916.c"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

//extern int8_t set_kw_offset(int16_t kw_id, int16_t offset);

// int8_t kws_buffer[6144] __attribute__((aligned(4))) AT(.usr_data);
// int8_t buffer2[8736] __attribute__((aligned(4))) AT(.kws_data);
//int8_t kws_buffer[2840 + 10528] __attribute__((aligned(4))); // v4

int8_t raw_buffer[128 * 5 * 2] __attribute__((aligned(4))) AT(HSuo_aec_data);
int32_t med_buffer[512] __attribute__((aligned(4))) AT(HSuo_aec_data);

AT(.audio_a.text.cache.L2)
__attribute__((weak)) uint32_t norflash_get_uuid(uint8_t *uuid)
{
    extern u8 *tzflash_get_uuid(void);

    memcpy(uuid, tzflash_get_uuid(), 16);

    return 0;
}

AT(.audio_a.text.cache.L2)
__attribute__((weak))  u32 sys_cfg_read_otp(u32 id, u8 *buf, u32 len)
{
    extern u32 syscfg_read_otp(u32 id, u8 * buf, u32 len);

    return syscfg_read_otp(id, buf, len);
}

uint8_t kws_enable = 0;
/* KWS_STATUS kws_ret = 0; */
void kws_task_cfg(void)
{
    int32_t buf_size, buf2_size;
    getBufferSize(&buf_size, &buf2_size);
    log_info("\n\nbuffersize: %d + %d \n\n\r", buf_size, buf2_size);

    // KWS_STATUS ret = kws_init(kws_buffer, sizeof(kws_buffer), buffer2, sizeof(buffer2));
    KWS_STATUS ret = kws_init(raw_buffer, sizeof(raw_buffer), (int8_t *)med_buffer, sizeof(med_buffer));
    log_info("kws_init_ret: %d\n\n\r", ret);
    if (ret == errKWS_NoError) {
        kws_enable = 1;
    }

    char *version;
    version = kws_get_version();
    log_info("Version: %s\n\n\r", version);


//    gpio_set_mode(GPIOA, BIT(9), PORT_OUTPUT_HIGH);
//    gpio_set_mode(IO_PORT_SPILT(IO_PORTA_04), PORT_OUTPUT_LOW);
    power_on_task_handler();
}

typedef enum {KWS_SLEEPED, KWS_AWAKE, KWS_TO_SLEEP} kws_sleep_sta_t;
static struct _kws_handle {
    kws_sleep_sta_t sta;
    uint16_t tick;
} kws_handle = {0};

static void kws_sleep_fun(void)
{
    kws_handle.tick++;
    if (kws_handle.tick == (WAKE_STEPS * 1000 / 1)) {
        kws_handle.tick = 0;
        kws_handle.sta = KWS_TO_SLEEP;
        unregist_2ms_fun(KWS_SLEEP_FUN_INDEX);
    }
}

static void kws_refresh(void)
{
    kws_handle.tick = 0;

    if (kws_handle.sta != KWS_AWAKE) {
        regist_2ms_fun(KWS_SLEEP_FUN_INDEX, kws_sleep_fun);
    }
    kws_handle.sta = KWS_AWAKE;
}

//AT(.audio_a.text.cache.L2)
void kws_process()
{
    int retval = 0;
    int16_t top = -1;
    int16_t prob = -1;

//    putchar('w');
    if (kws_enable) {
        if (kws_handle.sta == KWS_TO_SLEEP) {
            kws_handle.sta = KWS_SLEEPED;
            sleep_task_handler();
        }


//        putchar('k');
//        gpio_write(IO_PORTA_04, 1);
        retval = kws_classify(&top, &prob);
        if (errKWS_InvalidLicense == retval) {
            log_print("errKWS_InvalidLicense\r\n");
        }
//        gpio_write(IO_PORTA_04, 0);

        if ((top >= 0) && (prob >= KW_TRG_MODE[top])) {
            log_print("Cmd:%d\nOutput: %s (%d)\n", top, kw_string2[top], prob);
//            kws_reset_decoding();

            if ((top > NUM_WW) && (kws_handle.sta == KWS_SLEEPED)) {
                return;
            }
            kws_refresh();
            /* post_msg(1, MSG_PLAY_RELPY); */
        }
    }
}
