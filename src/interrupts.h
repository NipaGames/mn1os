#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"

void idt_set(uint8_t index, void (*handler)(), uint16_t selector, uint8_t priviledge, uint8_t type);
void idt_init();

#endif
