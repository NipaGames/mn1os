#include "terminal.h"
#include "isr.h"
#include "idt.h"
#include "irq.h"
#include "pit.h"
#include "keyboard.h"
#include "cli.h"
#include "io.h"

void kernel_main() {
    t_init();
    t_set_pos(0, t_search_next_free_ln());
    t_clear();

    t_write("Now in the kernel!\n");
    // t_write("Nyt käyttöjärjestelmän ytimessä!\n");
    t_write("welcome :3\n");
    // t_write("tervetuloa :3\n");
    t_newline();

    isr_init();
    idt_init();
    irq_init();
    pit_init();
    kb_init();
    t_hook_keyboard();

    t_write("Use 'help' to list all commands.\n");
    t_newline();
    // t_write("Paina mitä tahansa näppäintä jatkaaksesi.\n");
    t_sync_cursor_pos();
    t_set_cursor(TEXT_MODE_CURSOR_UNDERLINE);
    enter_cli();

    // power off
    t_newline();
    t_write("Press any key to reboot.\n");
    // t_write("Paina mitä tahansa näppäintä käynnistääksesi tietokoneen uudestaan.\n");
    t_wait_for_input();
    reboot();
}

