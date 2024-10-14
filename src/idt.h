#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"

typedef enum {
    IDT_FLAG_GATE_TASK          = 0x5,
    IDT_FLAG_GATE_INT_16        = 0x6,
    IDT_FLAG_GATE_TRAP_16       = 0x7,
    IDT_FLAG_GATE_INT_32        = 0xE,
    IDT_FLAG_GATE_TRAP_32       = 0xF,

    IDT_FLAG_RING_0             = 0 << 5,
    IDT_FLAG_RING_1             = 1 << 5,
    IDT_FLAG_RING_2             = 2 << 5,
    IDT_FLAG_RING_3             = 3 << 5,

    IDT_FLAG_PRESENT            = 0x80,

} IDT_FLAGS;

void idt_set(uint8_t index, void (*handler)(), uint16_t selector, uint8_t flags);
void idt_init();

#endif
