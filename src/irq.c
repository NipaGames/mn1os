#include "irq.h"
#include "pic.h"
#include "io.h"
#include "isr.h"
#include "terminal.h"

#define PIC_OFFSET 0x20

void irq_handle(registers* regs) {
    int irq = regs->interrupt - PIC_OFFSET;

    t_write("unhandled irq lol\n");

    pic_end_interrupt(irq);
}

void irq_init() {
    pic_init(PIC_OFFSET, PIC_OFFSET + 8);
    for (int i = 0; i < 16; i++) {
        isr_set_handler(PIC_OFFSET + i, irq_handle);
    }
    sti();
}
