#include "isr.h"
#include "idt.h"
#include "io.h"
#include "terminal.h"
#include "memory.h"

// defined in isr_stubs.s
extern void* isr_stub_table[];

interrupt_handler g_isr_handlers[48];

void isr_init() {
    memset(&g_isr_handlers, 0, sizeof(g_isr_handlers));
    for (int i = 0; i < 256; i++) {
        idt_set(i, isr_stub_table[i], 0x08, IDT_FLAG_PRESENT | IDT_FLAG_GATE_INT_32 | IDT_FLAG_RING_0);
    }
}

void isr_assign_handler(int interrupt, interrupt_handler handler) {
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
    int isr = regs->interrupt;
    if (g_isr_handlers[isr] != 0) {
        g_isr_handlers[isr](regs);
    }
    else if (isr >= 32) {
        T_WRITE_DIAGNOSTIC_STUB();
        t_write("unhandled isr lol (isr ");
        t_write_dec(isr);
        t_write(")\n");
    }
    else {
        t_write("KERNEL PANIC: ");
        t_write(g_isr_exceptions[isr]);
        halt();
    }
}
