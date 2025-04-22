#ifndef _UART_DEBUG_
#define _UART_DEBUG_
#include "typedef.h"
// #include "wdt.h"

#define UART_LOG    1

#define     DEBUG_UART_NUM      0           //需与DEBUG_UART对齐
#define     DEBUG_UART          JL_UART0    //需与DEBUG_UART_NUM对齐

#if UART_LOG
void log_uart_init(u32 fre);
void uart_uninit(void);
int putchar_in_ram(char a);
int putchar(int a);
int getchar(void);
int puts(const char *out);

void put_u32hex(u32 dat);
void put_u32hex0(u32 dat);
void put_u32d(u32 dat);
void put_u16hex0(u16 dat);
void put_u16hex(u16 dat);
void put_u8hex(u8 dat);
void printf_buf(u8 *buf, u32 len);
void put_buf(u8 *buf, u32 len);
#define my_puts(a) puts(a)

#else

#define log_uart_init(...)
#define debug_puts(...)
#define printf_buf(...)
#define put_u32hex(...)
#define put_u16hex(...)
#define put_u8hex(...)
#define put_buf(...)
// #define putchar(...)
int putchar_in_ram(char a);
int putchar(int a);
char get_byte(void);
#define my_puts(...)
#endif

#endif


