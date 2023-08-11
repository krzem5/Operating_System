#ifndef _KERNEL_ISR_ISR_H_
#define _KERNEL_ISR_ISR_H_ 1
#include <kernel/types.h>



#define ISR_STACK_SIZE 64



void isr_init(void);



u8 isr_allocate(void);



_Bool isr_wait(u8 irq);



#endif
