#include "terminal.h"
#include "isr.h"
#include "idt.h"
#include "irq.h"

void timer() {
    // t_putc('.');
}

void kernel_main() {
    t_init();
    t_set_pos(0, t_search_next_free_ln());

    isr_init();
    idt_init();
    irq_init();
    irq_assign_handler(0, timer);

    t_write("Now in the kernel!\n");
    t_newline();
    t_write("welcome :3\n");
    t_newline();

    t_write("Press any key to continue.\n");
    t_sync_cursor_pos();
    t_set_cursor(TEXT_MODE_CURSOR_UNDERLINE);
}

