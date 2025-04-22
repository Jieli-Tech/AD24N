/* #include "includes.h" */
#include "uart.h"
/* #include "malloc.h" */
#include "gpio.h"

/* #define LOG_TAG_CONST   UART */
#define LOG_TAG         "[uart_demo]"
#include "log.h"

static void uart_irq_func(int uart_num, enum uart_event event)
{
    if (event & UART_EVENT_TX_DONE) {
        log_info("uart[%d] tx done", uart_num);
    }

    if (event & UART_EVENT_RX_DATA) {
        log_info("uart[%d] rx data", uart_num);
    }

    if (event & UART_EVENT_RX_TIMEOUT) {
        log_info("uart[%d] rx timerout data", uart_num);
    }

    if (event & UART_EVENT_RX_FIFO_OVF) {
        log_info("uart[%d] rx fifo ovf", uart_num);
    }
}

u8 uart_rx_ptr[128];
u8 frame_ptr[64];

#define TEST_UART_IDX 1

void uart_sync_demo_start(void)
{
    const struct uart_config config = {
        .baud_rate = 1000000,
        .tx_pin = IO_PORTA_02,
        .rx_pin = IO_PORTA_03,
        .parity = UART_PARITY_DISABLE,
        .tx_wait_mutex = 0,//1:不支持中断调用,互斥,0:支持中断,不互斥
    };

    log_info("uart_rx_ptr:%d", uart_rx_ptr);
    log_info("frame_ptr:%d", frame_ptr);

    const struct uart_dma_config dma = {
        .rx_timeout_thresh = 3 * 10000000 / config.baud_rate, //单位:us,公式：3*10000000/baud(ot:3个byte时间)
        .event_mask = UART_EVENT_TX_DONE | UART_EVENT_RX_TIMEOUT | UART_EVENT_RX_FIFO_OVF,
        .irq_callback = uart_irq_func,
        .irq_priority = 3,
        .rx_cbuffer = uart_rx_ptr,
        .rx_cbuffer_size = 128,
        .frame_size = 64,//=rx_cbuffer_size
    };

    log_info("************uart demo***********");
    uart_dev uart_id = TEST_UART_IDX;
    int r;
    int ut = uart_init(uart_id, &config);
    if (ut < 0) {
        log_error("uart(%d) init error\n", ut);
    } else {
        log_info("uart(%d) init ok", ut);
    }
    r = uart_dma_init(uart_id, &dma);
    if (r < 0) {
        log_error("uart(%d) dma init error\n", ut);
    } else {
        log_info("uart(%d) dma init ok", ut);
    }
    uart_dump();
}

void uart_sync_demo_run(void)
{
    int r;
    if (frame_ptr) {
        r = uart_recv_blocking(TEST_UART_IDX, frame_ptr, 64, 10);
        if (r > 0) { //ok
            log_info("r len:%d", r);
            printf_buf((u8 *)frame_ptr, r);
            r = uart_send_blocking(TEST_UART_IDX, frame_ptr, r, 20);
            memset(frame_ptr, 0, sizeof(frame_ptr));
        }
    }
}

void uart_sync_demo_stop(void)
{
    uart_deinit(TEST_UART_IDX);
}

