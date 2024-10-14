#include "idt.h"

typedef struct {
    uint16_t handler_low;
    uint16_t selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t handler_high;
} __attribute__((packed)) idt_entry;

typedef struct {
    uint16_t size;
    uint32_t base;
} __attribute__((packed)) idt_pointer;

idt_entry g_idt_entries[256];
idt_pointer g_idt_pointer;

void idt_set(uint8_t index, void (*handler)(), uint16_t selector, uint8_t flags) {
    g_idt_entries[index] = (idt_entry) {
        .handler_low = ((uint32_t) handler) & 0xffff,
        .handler_high = ((uint32_t) handler >> 16) & 0xffff,
        .selector = selector,
        .flags = flags,
        .reserved = 0
    };
}

void idt_init() {
    g_idt_pointer.size = sizeof(g_idt_entries) - 1;
    g_idt_pointer.base = (uint32_t) &g_idt_entries[0];
    __asm__ volatile ("lidt %0" : : "m" (g_idt_pointer));
}

