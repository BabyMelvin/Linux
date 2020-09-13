#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

typedef void (*irq_func)(int);
void register_irq (int irq, irq_func fp);
#endif
