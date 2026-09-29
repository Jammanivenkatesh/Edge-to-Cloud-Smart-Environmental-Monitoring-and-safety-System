#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "types.h"

extern volatile u8 admin_mode_requested;
extern volatile u8 upload_timer_flag;
extern volatile u32 minute_counter;

void Configure_Interrupts(void);
void eint0_isr(void) __irq;
void rtc_ciir_isr(void) __irq;

#endif
