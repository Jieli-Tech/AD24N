#ifndef HWI_H
#define HWI_H

#include "typedef.h"

#define IRQ_EMUEXCPT_IDX   0
#define IRQ_EXCEPTION_IDX  1
#define IRQ_SYSCALL_IDX    2
#define IRQ_TICKTMR_IDX    3

#define IRQ_SOFT0_IDX      4
#define IRQ_SOFT1_IDX      5
#define IRQ_SOFT2_IDX      6
#define IRQ_SOFT3_IDX      7
#define IRQ_SOFT4_IDX      8
#define IRQ_SOFT5_IDX      9
#define IRQ_SOFT6_IDX      10
#define IRQ_SOFT7_IDX      11

#define IRQ_TIME0_IDX      12
#define IRQ_TIME1_IDX      13
#define IRQ_TIME2_IDX      14
#define IRQ_UART0_IDX      15
#define IRQ_UART1_IDX      16
#define IRQ_SPI1_IDX       17
#define IRQ_SPI2_IDX       18
#define IRQ_IIC0_IDX        19
#define IRQ_PORT_IDX       20
#define IRQ_GPADC_IDX      21
#define IRQ_LRCT_IDX       22
#define IRQ_GPCNT_IDX      23
#define IRQ_SD0_IDX 	   24
#define IRQ_USB_SOF_IDX    25
#define IRQ_USB_CTRL_IDX   26
#define IRQ_SD0_BRK_IDX    27
#define IRQ_MCPWM_TMR_IDX   28
#define IRQ_MCPWM_CHX_IDX   29

#define IRQ_PMU_SOFT0_IDX       30
#define IRQ_PMU_SOFT1_IDX       31
#define IRQ_PMU_SOFT2_IDX       32
#define IRQ_PMU_SOFT3_IDX       33

#define IRQ_PMU0_IDX       34
#define IRQ_PMU1_IDX       35
#define IRQ_PMU2_IDX       36
#define IRQ_PMU3_IDX       37
#define IRQ_PMU4_IDX       38

#define IRQ_SRC_HW_IDX     41
#define IRQ_SPI0_IDX       42

#define IRQ_ALNK_SOFT      44
#define IRQ_APA_SOFT       45
#define IRQ_ADC_SOFT       46
#define IRQ_DAC_SOFT       47

#define MAX_IRQ_ENTRY_NUM 48

/* #define IRQ_EMUEXCPT_IDX   0      */
/* #define IRQ_EXCEPTION_IDX  1      */
/* #define IRQ_SYSCALL_IDX    2      */
/* #define IRQ_TICKTMR_IDX    3      */
/* #define IRQ_TIME0_IDX      4      */
/* #define IRQ_TIME1_IDX      5      */
/* #define IRQ_TIME2_IDX      6      */
/* // #define IRQ_P33_IDX        8   */
/* // #define IRQ_AUDIO_IDX        9 */
/* #define IRQ_UART0_IDX      10     */
/* #define IRQ_UART1_IDX      11     */
/* #define IRQ_SPI0_IDX       13     */
/* #define IRQ_SPI1_IDX       14     */
/* #define IRQ_IIC_IDX        17     */

/* #define IRQ_PMU_SODT_IDX   20     */
/* #define IRQ_PORT_IDX       25     */
/* #define IRQ_GPADC_IDX      26     */

/* #define IRQ_OSA_IDX        28     */
/* #define IRQ_LRCT_IDX       29     */
/* #define IRQ_GPCNT_IDX      30     */

/* #define IRQ_MCPWM_CHX_IDX   38    */
/* #define IRQ_MCPWM_TMR_IDX   39    */
/* #define IRQ_APA_IDX         46    */
/* #define IRQ_SRC_HW_IDX      49    */

/* #define IRQ_SOFT0_IDX      60     */
/* #define IRQ_SOFT1_IDX      61     */
/* #define IRQ_SOFT2_IDX      62     */
/* #define IRQ_SOFT3_IDX      63     */

#define IRQ_AUDIO_IDX      IRQ_DAC_SOFT
#define IRQ_AUDIO_B_IDX    IRQ_ADC_SOFT


//系统使用到的
extern const int IRQ_IRTMR_IP;
extern const int IRQ_AUDIO_IP;
extern const int IRQ_AUDAC_IP;
extern const int IRQ_AUAPA_IP;
extern const int IRQ_AUADC_IP;
extern const int IRQ_DECODER_IP;
extern const int IRQ_WFILE_IP;
extern const int IRQ_ADC_IP;
extern const int IRQ_ENCODER_IP;
extern const int IRQ_TICKTMR_IP;
extern const int IRQ_USB_IP;
extern const int IRQ_SD_IP;
//系统还未使用到的
extern const int IRQ_UART0_IP;
extern const int IRQ_UART1_IP;
extern const int IRQ_ALINK0_IP;



extern u32 _IRQ_MEM_ADDR[];

#define IRQ_MEM_ADDR        (_IRQ_MEM_ADDR)

void bit_clr_swi(unsigned char index);
void bit_set_swi(unsigned char index);

void bit_set_swi0(void);

void interrupt_init();

void irq_save(u32 b_index_l, u32 b_index_h);
void irq_resume(void);
void irq_enable(u8 index);
void reg_set_ip(unsigned char index, unsigned char priority);
void HWI_Install(unsigned char index, unsigned int isr, unsigned char priority);

extern void request_irq(u8 index, u8 priority, void (*handler)(void), u8 cpu_id);
void unrequest_irq(u8 index);

void bit_clr_ie(unsigned char index);
void bit_set_ie(unsigned char index);

void irq_unmask_set(u8 index);
void irq_unmask_disable(u8 index);

#ifdef IRQ_TIME_COUNT_EN
void irq_handler_enter(int irq);

void irq_handler_exit(int irq);

void irq_handler_times_dump();
#else

#define irq_handler_enter(irq)      do { }while(0)
#define irq_handler_exit(irq)       do { }while(0)
#define irq_handler_times_dump()    do { }while(0)

#endif


static inline int core_num(void)
{
    return 0;
}
#endif

