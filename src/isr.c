#include "isr.h"
#include "idt.h"
#include "terminal.h"

// defined in isrstubs.s
extern void* isr_stub_table[];

void isr_init() {
    for (int i = 0; i < 32; i++) {
        idt_set(i, isr_stub_table[i], 0x08, IDT_FLAG_PRESENT | IDT_FLAG_GATE_INT_32 | IDT_FLAG_RING_0);
    }
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
    if (regs->interrupt >= 32)
        t_write("interrupt not handled yet lol\n");
    else {
        t_write("PANIC: ");
        t_write(g_isr_exceptions[regs->interrupt]);
        __asm__ volatile ("hlt");
    }
}
