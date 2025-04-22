#ifndef __INIT_H__
#define __INIT_H__

void emu_init();
void exception_analyze(unsigned int *sp);
void emu_test();
void stack_protect_set(u32 enable, u32 stack_begin_addr, u32 stack_end_addr);
extern void call_by_change_stack(u32, void *, void *);
extern int _cpu0_sstack_begin;
extern int _cpu0_sstack_end;

void early_system_init(void);
void system_init(void);
void all_init_isr(void);
#endif
