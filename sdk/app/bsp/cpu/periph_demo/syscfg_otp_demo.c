/* #include "includes.h" */
#include "uart.h"
#include "gpio.h"
#include "tzflash_api.h"

/* #define LOG_TAG_CONST   UART */
#define LOG_TAG         "[otp_demo]"
#include "log.h"
int syscfg_check_otp_uuid()
{
    log_info("*******check otp uuid*******");
    int ret = 0;
    u32 uuid_otp_id = 123;//Authorization_ID 授权的id
    u32 uuid_otp_len = 17;//Authorization_ID 对应的len
    u8 enc_data[17] = {0};
    u8 cal_enc_data[17] = {0};
    int r = syscfg_read_otp(uuid_otp_id, enc_data, uuid_otp_len);
    log_info("read otp data:");
    put_buf(enc_data, sizeof(enc_data));
    if (r != uuid_otp_len) {
        log_error("read otp error %d\n", r);
        return -1;
    }
    u8 *uuid;
    uuid = tzflash_get_uuid();
    log_info("read uuid data:");
    put_buf(uuid, 16);

    //校验授权数据是否合法
    for (int i = 0; i < r; i++) {
        cal_enc_data[i] = (i ^ uuid[i % 16]);
        if (enc_data[i] != cal_enc_data[i]) {
            log_error("check error %d %x %x", i, enc_data[i], i ^ uuid[i % 16]);
            // 当授权数据不合法的时候，需要退出
            /* return; */
            ret = -2;
        }
    }
    log_info("generate enc data:(=otp)");
    put_buf(cal_enc_data, sizeof(cal_enc_data));
    if (ret == 0) {
        log_info("check otp uuid ok!");
    } else {
        log_error("check otp uuid fail!");
    }
    return ret;
}

void tzflash_write_protect_test(u32 test_addr)
{
    /******************写保护功能测试**********************/
    /* test_addr = boot_info.vm.vm_saddr + 0x1000;//vm区测试 */
    u32 flash_size;
    log_info("*********norflash write protect test: addr:0x%x*********\n", test_addr);
    tzflash_ioctl(NULL, IOCTL_GET_CAPACITY, (u32)(&flash_size));
    if (test_addr > flash_size) {
        log_info("test_addr out of range!\n");
        return;
    }
    tzflash_set_write_protect(0, 0, 0xffffffff);//全解保护
    int wp_end_addr = tzflash_set_write_protect(1, 0, test_addr);
    tzflash_dump();

    const u32 test_addr_len = 32;
    u8 w_buf[test_addr_len];
    u8 r_buf[test_addr_len];

    memset(w_buf, 0, sizeof(w_buf));
    memset(r_buf, 0xff, sizeof(r_buf));
    tzflash_write(NULL, w_buf, sizeof(w_buf), 0);
    tzflash_origin_read(NULL, r_buf, sizeof(r_buf), 0);
    if (memcmp(w_buf, r_buf, sizeof(w_buf)) == 0) {
        log_error("norflash write protect fail!test addr:0\n");
        put_buf(w_buf, sizeof(w_buf));
        put_buf(r_buf, sizeof(r_buf));
        while (1);
    }
    if (test_addr == 0) {
        log_info("norflash write protect succ!\n");
        return;
    }
    log_info("norflash write protect ok!test addr:0\n");

    u32 wp_addr[2] = {test_addr, wp_end_addr - 4096};//实际地址
    for (u32 i = 0; i < 2; i++) {
        memset(w_buf, 0, sizeof(w_buf));
        memset(r_buf, 0xff, sizeof(r_buf));
        tzflash_ioctl(NULL, IOCTL_ERASE_SECTOR, wp_addr[i]);
        tzflash_write(NULL, w_buf, sizeof(w_buf), wp_addr[i]);
        tzflash_origin_read(NULL, r_buf, sizeof(r_buf), wp_addr[i]);
        if (i == 0) {
            if (memcmp(w_buf, r_buf, sizeof(w_buf)) != 0) {//未保护
                log_error("norflash write protect fail!test addr:0x%x\n", wp_addr[i]);
                put_buf(w_buf, sizeof(w_buf));
                put_buf(r_buf, sizeof(r_buf));
                while (1);
            } else {
                log_info("norflash write protect ok!test addr:0x%x\n", wp_addr[i]);
            }
        }

        if (i == 1) {
            if (memcmp(w_buf, r_buf, sizeof(w_buf)) == 0) {//保护
                log_error("norflash write protect fail!test addr:0x%x\n", wp_addr[i]);
                put_buf(w_buf, sizeof(w_buf));
                put_buf(r_buf, sizeof(r_buf));
                while (1);
            } else {
                log_info("norflash write protect ok!test addr:0x%x\n", wp_addr[i]);
            }
        }
    }
    log_info("norflash write protect succ!\n");
    while (1) {
        wdt_clear();
    }
}

