#include "includes.h"
#include "app_modules.h"
//中断优先级
//系统使用到的
//*********************************************************************************//
//                        ISR Configuration                                        //
//*********************************************************************************//
const int IRQ_IRTMR_IP   = 5;
const int IRQ_AUDIO_IP   = 4;
const int IRQ_AUDAC_IP   = 4;
const int IRQ_AUAPA_IP   = 4;
const int IRQ_AUADC_IP   = 4;
const int IRQ_DECODER_IP = 1;
const int IRQ_WFILE_IP   = 1;
const int IRQ_ADC_IP     = 2;
const int IRQ_ENCODER_IP = 0;
const int IRQ_TICKTMR_IP = 3;
const int IRQ_USB_IP     = 2;
const int IRQ_SD_IP      = 3;
const int IRQ_PMU_TIMER1_IP = 0;
//系统还未使用到的
const int IRQ_UART0_IP   = 3;
const int IRQ_UART1_IP   = 3;
const int IRQ_ALINK0_IP  = 3;

//*********************************************************************************//
//                        vm_sfc Configuration                                     //
//*********************************************************************************//
const u8 config_spi_code_user_cache = 1;//sfc放code区

#if (CONFIG_FLASH_DTR_EN)
const u8 sfc0_dtr_mode_en = 1;
#else
const u8 sfc0_dtr_mode_en = 0;
#endif
const s8 sfc0_dtr_dummy_num = -1;//查看flash文档填入ini对应线宽的dummy
const u32 sfc0_dtr_clk_freq = 192000000;//查看flash文档*2
const u8 sfc0_continue_mode_en = 0;


//*********************************************************************************//
//                        malloc Configuration                                     //
//*********************************************************************************//
const char MM_ASSERT     = TRUE;  //malloc内部断言


//*********************************************************************************//
//                        midi Configuration                                       //
//*********************************************************************************//
#if (DECODER_MIDI_EN | DECODER_MIDI_KEYBOARD_EN)
const int MAINTRACK_USE_CHN  = 0;    //0:用track号来区分  1:用channel号来区分。
const int MAX_DEC_PLAYER_CNT = 8;    //midi乐谱解码最大同时发声的key数,范围[1,31]
const int MAX_CTR_PLAYER_CNT = 8;    //midi琴最大同时发声的key数,范围[1,31]
const int NOTE_OFF_TRIGGER   = 0;    //midi琴note_off回调 1：time传0时，不会回调 0：time传0时，回调
const int MIDI_MAX_MARK_CNT  = 0;    //midi解码最大支持的mark数
#endif


//*********************************************************************************//
//                        update Configuration                                     //
//*********************************************************************************//
//升级使用的区域，0：VM区， 1：eeprom区
const u8 dev_update_use_eeprom = 0;
//设备升级时，是否保持住io的状态
const u8 dev_update_keep_io_status = 0;
//设备升级时，用到的电源引脚
const u8 dev_update_power_io = -1;
//ufw升级文件的vid要求： 0：vid要相同  1：vid要不一样 2：升级文件vid > 当前设备vid 3：升级文件vid < 当前设备vid
const u8 ufw_vid_need_to_be_different = 0;
//sd空闲后挂起的最大cnt值，单位时间是sd检测函数的时间，即sd空闲后每次检测函数cnt就加1，为0时，则每次读写完都会发挂起命令


//*********************************************************************************//
//                        src Configuration                                        //
//*********************************************************************************//
//软件重采样quality影响运算速度,可选范围3~8,默认选8
const u8 resample_quality  = 8;


//*********************************************************************************//
//                        sdmmc Configuration                                      //
//*********************************************************************************//
//sd空闲后挂起的最大cnt值，单位时间是sd检测函数的时间，即sd空闲后每次检测函数cnt就加1，为0时，则每次读写完都会发挂起命令
const u8 is_sdx_active_cnt_max = 20;


//*********************************************************************************//
//                        UART Configuration                                       //
//*********************************************************************************//
const u8 lib_config_uart_flow_enable = 0;
//浮点打印，仅仅作用在AD17N,AD18N
const int printf_support_float = 0;


//*********************************************************************************//
//                        debug Configuration                                      //
//*********************************************************************************//
const u8 config_asser = 1;      //0:异常复位；1:异常打印卡死



//*********************************************************************************//
//                        gptimer Configuration                                      //
//*********************************************************************************//
const u8 lib_gptimer_src_lsb_clk = 0; //时钟源选择lsb_clk, 单位:MHz
const u8 lib_gptimer_src_std_clk = 12; //时钟源选择std_x_clk, 单位:MHz
const u8 lib_gptimer_timer_mode_en = 1; //gptimer timer功能使能
const u8 lib_gptimer_pwm_mode_en = 1; //gptimer pwm功能使能
const u8 lib_gptimer_capture_mode_en = 1; //gptimer capture功能使能
const u8 lib_gptimer_auto_tid_en = 1; //gptimer_tid 内部自动分配使能

/**
 * @brief Controller Log
 */
/*-----------------------------------------------------------*/

const char libs_debug AT(.LOG_TAG_CONST) = TRUE; //打印总开关

#define  CONFIG_DEBUG_LIBS(X)   (X & libs_debug)

const char log_tag_const_i_MAIN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_MAIN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_MAIN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_KEYM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_KEYM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_KEYM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_MUGRD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_MUGRD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_MUGRD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_PWRA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_PWRA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_PWRA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_e_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);


const char log_tag_const_i_P33 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_P33 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_P33 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);


const char log_tag_const_i_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_RST AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_RST AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_RST AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);


const char log_tag_const_i_SOFI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_SOFI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_SOFI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);


const char log_tag_const_i_UTD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_UTD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_UTD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_c_UTD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);


const char log_tag_const_i_LBUF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_LBUF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_LBUF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);


const char log_tag_const_i_NORM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_NORM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_NORM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_NORM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_c_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_FLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_FLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_FLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_FLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_TZFLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_TZFLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_TZFLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_TZFLASH AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_SPI1 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_SPI1 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_SPI1 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_SPI1 AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_SPI1_TEST AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_SPI1_TEST AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_SPI1_TEST AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_EEPROM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_EEPROM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_EEPROM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_EEPROM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);


const char log_tag_const_i_HEAP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_HEAP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_HEAP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_c_HEAP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_CPU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_d_CPU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_CPU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_c_CPU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0) ;
const char log_tag_const_d_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0) ;
const char log_tag_const_d_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0) ;
const char log_tag_const_d_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);

const char log_tag_const_i_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0) ;
const char log_tag_const_d_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0) ;
const char log_tag_const_d_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_e_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_GPTIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0) ;
const char log_tag_const_d_GPTIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_GPTIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_GPTIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);

const char log_tag_const_i_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_d_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
const char log_tag_const_e_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(1);
const char log_tag_const_c_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_LIBS(0);
