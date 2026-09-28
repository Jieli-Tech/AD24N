#ifndef __BOARD_CFG_H__
#define __BOARD_CFG_H__
#include "app_modules.h"

//*********************************************************************************//
//                        UART Configuration                                       //
//*********************************************************************************//
#define UART_OUTPUT_CH_PORT         IO_PORTA_04


//*********************************************************************************//
//                        KEY Configuration                                        //
//*********************************************************************************//
//AD KEY
#define AD_KEY_CH_SEL               ADC_CH_PA1
//IR KEY
#define IR_KEY_IO_SEL               IO_PORTA_09
#define TCFG_ADKEY_IR_IO_REUSE      DISABLE//ADKEY 和 红外IO复用
//MATRIX KEY
///X轴 io 要求是AD口，详细AD口看adc_drv.h
#define X_ADC_CH_SEL                {ADC_CH_PA2,ADC_CH_PA3,ADC_CH_PA5}
///Y轴 io 要求是普通IO口
#define Y_PORT_SEL                  {IO_PORTA_10,IO_PORTA_11}
//TOUCH KEY
#define TOUCH_KEY_SEL               {IO_PORTA_09,IO_PORTA_10,IO_PORTA_11}


//*********************************************************************************//
//                       POWER WAKEUP IO Configuration                             //
//*********************************************************************************//
#define POWER_WAKEUP_IO             IO_PORTA_01


//*********************************************************************************//
//                       EXFLASH PORT Configuration                                //
//*********************************************************************************//
//port select for hardware spi
//support any io
#define EXFLASH_CS_PORT_SEL         IO_PORTA_05
#define EXFLASH_CLK_PORT_SEL        IO_PORTA_11
#define EXFLASH_D0_PORT_SEL         IO_PORTA_12
#define EXFLASH_D1_PORT_SEL         IO_PORTA_10


//port select for soft spi
//support any io
#define A_CLK_BIT                   BIT(12)// set clk
#define A_CLK_PORT(x)               JL_PORTA->x
#define A_D0_BIT                    BIT(11)// set d0
#define A_D0_PORT(x)                JL_PORTA->x
#define A_D1_BIT                    BIT(10)// set d1
#define A_D1_PORT(x)                JL_PORTA->x


//*********************************************************************************//
//                       SDMC PORT Configuration                                //
//*********************************************************************************//
#define SDMMC_IO_USE_CROSSBAR       1

//support any io
#define SDMMC_CLK_IO                IO_PORTA_15
#define SDMMC_CMD_IO                IO_PORTA_02
#define SDMMC_DAT_IO                IO_PORTA_03

#endif
