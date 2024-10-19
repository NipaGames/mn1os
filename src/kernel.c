#include "terminal.h"
#include "isr.h"
#include "idt.h"
#include "irq.h"
#include "pit.h"
#include "keyboard.h"
#include "cli.h"

void kernel_main() {
    t_init();
    t_set_pos(0, t_search_next_free_ln());
    t_clear();

    t_write("Now in the kernel!\n");
    t_newline();
    t_write("welcome :3\n");
    t_newline();

    isr_init();
    idt_init();
    irq_init();
    pit_init();
    kb_init();
    t_hook_keyboard();

    t_write("Press any key to enter the CLI.\n");
    t_wait_for_input();
    t_clear();
    enter_cli();

    // TODO: power off
    // t_newline();
    // t_write("Press any key to power off.\n");
    // t_wait_for_input();
}

