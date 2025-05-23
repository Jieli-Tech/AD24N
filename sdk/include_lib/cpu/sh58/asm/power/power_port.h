#ifndef __POWER_PORT_H__
#define __POWER_PORT_H__

//
//
//					FLASH PIN
//
//
//
//*****************************************************************************/
#define GET_SFC_PORT()		(JL_IOMC->SYS_IOMC1 & 0x1)

/******************************************************************************/
#define     _PORT(p)            JL_PORT##p
#define     _PORT_IN(p,b)       P##p##b##_IN
#define     _PORT_OUT(p,b)      JL_OMAP->P##p##b##_OUT

/****************************spi boot *****************************************/
#define     SPI_PORT(p)         _PORT(p)
#define     SPI0_FUNC_OUT(p,b)  _PORT_OUT(p,b)
#define     SPI0_FUNC_IN(p,b)   _PORT_IN(p,b)
// | func\port |  A   | B    |
// |-----------|------|------|
// | VCC       | FSPG |  |
// | CS        | PD3  |  |
// | CLK       | PD0  |  |
// | DO(D0)    | PD1  |  |
// | DI(D1)    | PD2  |  |
// | WP(D2)    | PA5  |  |
// | HOLD(D3)  | PA6  |  |


//FSPG define bit
#define     FSPG_A              0   //FSPG output bit
#define     FSPG_CS_EN          1   //FSPG CS connect enable bit
#define     FSPG_HD0            2   //FSPG HD0
#define     FSPG_HD1            3   //FSPG HD1
#define     FSPG_OE             4   //FSPG output enable bit
#define     FSPG_PD0            5   //FSPG pull down enable bit
#define     FSPG_PD1            6   //FSPG pull down enable bit
#define     FSPG_18V            7   //flash supply power domain 1: DCVDD 1.8V; 0: IOVDD;

////////////////////////////////////////////////////////////////////////////////
//group a
#define     PORT_SPI0_CSA       F
#define     SPI0_CSA            3

#define     PORT_SPI0_CLKA      F
#define     SPI0_CLKA           1

#define     PORT_SPI0_DOA       F
#define     SPI0_DOA            0

#define     PORT_SPI0_DIA       F
#define     SPI0_DIA            2

#define     PORT_SPI0_D2A       A
#define     SPI0_D2A            7

#define     PORT_SPI0_D3A       B
#define     SPI0_D3A            3

//group b
#define     PORT_SPI0_CSB       F
#define     SPI0_CSB            0

#define     PORT_SPI0_CLKB      F
#define     SPI0_CLKB           2

#define     PORT_SPI0_DOB       F
#define     SPI0_DOB            3

#define     PORT_SPI0_DIB       F
#define     SPI0_DIB            1

#define     PORT_SPI0_D2B       B
#define     SPI0_D2B            3

#define     PORT_SPI0_D3B       A
#define     SPI0_D3B            7


////////////////////////////////////////////////////////////////////////////////
//#define		SPI0_PWR_A		IO_PORTD_04
#define		SPI0_CS_A		IO_PORTF_03
#define 	SPI0_CLK_A		IO_PORTF_01
#define 	SPI0_DO_D0_A	IO_PORTF_00
#define 	SPI0_DI_D1_A	IO_PORTF_02
#define 	SPI0_WP_D2_A	IO_PORTA_07
#define 	SPI0_HOLD_D3_A	IO_PORTB_03

////////////////////////////////////////////////////////////////////////////////
//PORTB
#define		SPI0_CS_B		IO_PORTF_00
#define 	SPI0_CLK_B		IO_PORTF_02
#define 	SPI0_DO_D0_B	IO_PORTF_03
#define 	SPI0_DI_D1_B	IO_PORTF_01
#define 	SPI0_WP_D2_B	IO_PORTB_03
#define 	SPI0_HOLD_D3_B	IO_PORTA_07

////////////////////////////////////////////////////////////////////////////////

//
//
//					PSRAM
//
//
//
//*****************************************************************************/
#define		PSRAM_D0A		IO_PORTC_00
#define		PSRAM_D1A		IO_PORTC_02
#define		PSRAM_D2A		IO_PORTC_01
#define		PSRAM_D3A		IO_PORTA_12

#define		PINR_DEFAULT_IO IO_PORTA_00

#define		MCLR_PORT		IO_PORTA_08

#define PORT_TABLE(arg)		u32 gpio_confi##arg[5] = {0xffff, 0xffff, 0xffff, 0xffff, 0xffff}

void port_protect(u32 *gpio_config, u32 gpio);

#define PORT_PROTECT(gpio)		port_protect(gpio_config, gpio)

void init_boot_rom();
u8 is_boot_rom();

void *__port_init(u32 arg);

#endif
