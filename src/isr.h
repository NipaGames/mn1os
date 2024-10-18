#ifndef ISR_H
#define ISR_H

#include "types.h"

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t interrupt, error;
    uint32_t eip, cs, eflags, esp_usr, ss;
} __attribute__((packed)) registers;

typedef void (*interrupt_handler)(registers* regs);

void isr_init();
void isr_assign_handler(int interrupt, interrupt_handler handler);

#endif
