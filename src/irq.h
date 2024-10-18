#ifndef IRQ_H
#define IRQ_H

#include "isr.h"

void irq_init();
void irq_assign_handler(int interrupt, interrupt_handler handler);

#endif
