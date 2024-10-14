#include "isr.h"
#include "idt.h"
#include "io.h"
#include "terminal.h"

// defined in isr_stubs.s
extern void* isr_stub_table[];

isr_handler g_isr_handlers[48] = { 0 };

void isr_init() {
    for (int i = 0; i < 256; i++) {
        idt_set(i, isr_stub_table[i], 0x08, IDT_FLAG_PRESENT | IDT_FLAG_GATE_INT_32 | IDT_FLAG_RING_0);
    }
}

void isr_set_handler(int interrupt, isr_handler handler) {
    g_isr_handlers[interrupt] = handler;
}

static const char* g_isr_exceptions[32] = {
    "Division Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    ""
};

void isr_handle_interrupt(registers* regs) {
    if (g_isr_handlers[regs->interrupt] != 0) {
        g_isr_handlers[regs->interrupt](regs);
    }
    else if (regs->interrupt >= 32) {
        t_write("interrupt not handled yet lol\n");
    }
    else {
        t_write("PANIC: ");
        t_write(g_isr_exceptions[regs->interrupt]);
        halt();
    }
}
