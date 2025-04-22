#ifndef __APP_ICACHE_H__
#define __APP_ICACHE_H__
/*---------ICACHE RAM 相关配置-----------------*/
// #define ICACHE_RAM_RELEASS_NUMBER			2//ICACHE RAM用作普通RAM使能位

#ifdef ICACHE_RAM_RELEASS_NUMBER
#define ICACHE_RAM_TO_RAM                   (ICACHE_RAM_RELEASS_NUMBER * 4096)//将多少ICACHE RAM用作普通RAM----配置大小：4K、8K
#else
#define ICACHE_RAM_TO_RAM                   0//将多少ICACHE RAM用作普通RAM----配置大小：4K、8K
#endif
#ifdef ICACHE_RAM_RELEASS_NUMBER
#if ICACHE_RAM_RELEASS_NUMBER > 3
#error "ICACHE_RAM_RELEASS_NUMBER 超出范围 (0-3)"
#endif
#endif

#endif

