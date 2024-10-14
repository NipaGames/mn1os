#include "interrupts.h"

struct idt_entry {
    uint16_t handler_low;
    uint16_t selector;
    uint8_t reserved;
    uint8_t access;
    uint16_t handler_high;
} __attribute__((packed));

struct idt_pointer {
    uint16_t size;
    uint32_t base;
} __attribute__((packed));

struct {
    struct idt_entry entries[256];
    struct idt_pointer pointer;
} idt;

void idt_set(uint8_t index, void (*handler)(), uint16_t selector, uint8_t priviledge, uint8_t type) {
    idt.entries[index] = (struct idt_entry) {
        .handler_low = ((uint32_t) handler) & 0xffff,
        .handler_low = ((uint32_t) handler >> 16) & 0xffff,
        .selector = selector,
        .access = 0x80 | type | ((priviledge & 3) << 5)
    };
}

#include "terminal.h"
void idt_ignore() {
    t_write("INTERRUPT\n");
}

#include "io.h"
void idt_init() {
    idt.pointer.size = sizeof(idt.entries) - 1;
    idt.pointer.base = (uint32_t) &idt.entries[0];
    for (int i = 0; i < 256; i++) {
        idt_set(i, &idt_ignore, 0x08, 0, 0xe);
    }

    outb(0x20, 0x11);
    outb(0xa0, 0x11);

    outb(0x21, 0x20);
    outb(0xa1, 0x28);

    outb(0x21, 0x04);
    outb(0xa1, 0x02);

    outb(0x21, 0x01);
    outb(0xa1, 0x01);

    outb(0x20, 0x00);
    outb(0xa0, 0x00);

    asm volatile("lidt %0" : : "m" (idt.pointer));
}

uint32_t handle_interrupt(uint8_t interrupt, uint32_t esp) {
    t_write("INTERRUPT\n");
    return esp;
}
