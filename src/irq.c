#include "irq.h"
#include "pic.h"
#include "io.h"
#include "terminal.h"
#include "memory.h"

#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28

interrupt_handler g_irq_handlers[16];

void irq_assign_handler(int interrupt, interrupt_handler handler) {
    if (interrupt < 0 || interrupt > 15) {
        T_WRITE_DIAGNOSTIC_STUB();
        t_write("invalid interrupt! (0-15 allowed, ");
        t_write_dec(interrupt);
        t_write(" given)");
        return;
    }
    g_irq_handlers[interrupt] = handler;
}

void irq_handle(registers* regs) {
    int irq = regs->interrupt - PIC1_OFFSET;
    if (g_irq_handlers[irq] != 0) {
        g_irq_handlers[irq](regs);
    }
    else {
        T_WRITE_DIAGNOSTIC_STUB();
        t_write("unhandled irq lol (irq ");
        t_write_dec(irq);
        t_write(")\n");
    }
    pic_end_interrupt(irq);
}

void irq_init() {
    memset(&g_irq_handlers, 0, sizeof(g_irq_handlers));
    pic_init(PIC1_OFFSET, PIC2_OFFSET);
    for (int i = 0; i < 16; i++) {
        isr_assign_handler(PIC1_OFFSET + i, irq_handle);
    }
    sti();
}
