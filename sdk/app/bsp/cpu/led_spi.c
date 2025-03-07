#include "includes.h"
#include "clock.h"
#include "spi.h"


#define LOG_TAG_CONST NORM
#define LOG_TAG     "[led_spi]"
#include "log.h"


#define LED_SPI_DAT_BAUD        8000000
#define LED_SPI_REST_BAUD       1000000

static hw_spi_dev led_spi_dev;
static u8 led_spi_busy = 0;

void led_spi_isr_cbfunc(hw_spi_dev spi, enum hw_spi_isr_status sta)
{
    spi_set_ie(led_spi_dev, 0);
    led_spi_busy = 0;
}

void led_spi_init(hw_spi_dev spi, u32 spi_do)
{
    spi_hardware_info led_spi_pdata = {
        .port[0] = -1,
        .port[1] = spi_do,
        .port[2] = -1,
        .port[3] = -1,
        .port[4] = -1,
        .port[5] = -1,
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE_UNIDIR_1BIT,
        .bit_mode = SPI_FIRST_BIT_MSB,
        .cpol = 0,
        .cpha = 0,
        .ie_en = 1,
        .spi_isr_callback = led_spi_isr_cbfunc,
        .clk = LED_SPI_DAT_BAUD,
    };
    led_spi_dev = spi;
    spi_open(led_spi_dev, &led_spi_pdata);
    spi_send_byte(led_spi_dev, 0);
    spi_set_ie(led_spi_dev, 0);
}

void led_spi_rgb_to_24byte(u8 r, u8 g, u8 b, u8 *buf, int idx)
{
    buf = buf + idx * 24;
    u32 dat = ((g << 16) | (r << 8) | b);
    for (u8 i = 0; i < 24; i ++) {
        if (dat & BIT(23 - i)) {
            *(buf + i) = 0x7c;
        } else {
            *(buf + i) = 0x60;
        }
    }
}

void led_spi_rest()
{
    u8 tmp_buf[16] = {0};
    spi_set_baud(led_spi_dev, LED_SPI_REST_BAUD);
    spi_dma_send(led_spi_dev, (const void *)tmp_buf, 16);
}

void led_spi_send_rgbbuf(u8 *rgb_buf, u16 led_num) //rgb_buf的大小 至少要等于 led_num * 24
{
    if (!led_num) {
        return;
    }
    led_spi_busy = 1;
    led_spi_rest();
    spi_set_baud(led_spi_dev, LED_SPI_DAT_BAUD);
    spi_dma_send(led_spi_dev, (const void *)rgb_buf, led_num * 24);
    led_spi_busy = 0;
}

void led_spi_send_rgbbuf_isr(u8 *rgb_buf, u16 led_num) //rgb_buf的大小 至少要等于 led_num * 24
{
    if (!led_num) {
        return;
    }
    if (led_spi_busy) {
        return;
    }
    led_spi_busy = 1;
    led_spi_rest();
    spi_set_baud(led_spi_dev, LED_SPI_DAT_BAUD);
    spi_dma_transmit_for_isr(led_spi_dev, rgb_buf, led_num * 24, 0);
    spi_set_ie(led_spi_dev, 1);
}

static u8 spi_dat_buf[24 * 2] __attribute__((aligned(4)));
extern void wdt_clear();
void led_spi_test(void)
{
    log_info("******************  led spi test  *******************\n");

    led_spi_init(HW_SPI1, IO_PORTA_03);

    u8 cnt = 0;
    u8 pulse = 0;
    while (1) {
        cnt ++;
        led_spi_rgb_to_24byte(cnt, 255 - cnt, 0, spi_dat_buf, 0);
        led_spi_rgb_to_24byte(0, 0, cnt, spi_dat_buf, 1);
#if 0
        led_spi_send_rgbbuf(spi_dat_buf, 2);        //等待的方式，建议用在发的数据量小的场合
#else
        led_spi_send_rgbbuf_isr(spi_dat_buf, 2);    //中断的方式，建议用在发的数据量大的场合
#endif
        delay_10ms(2);
        wdt_clear();
    }
}

