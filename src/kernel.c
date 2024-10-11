#include "terminal.h"

void kernel_main() {
	t_init();
	t_set_pos(0, t_search_next_free_ln());

	t_write("Now in the kernel!\n");
	t_newline();
	t_write("welcome :3\n");
	t_newline();

	t_write("Press any key to continue.\n");
	t_sync_cursor_pos();
	t_set_cursor(TEXT_MODE_CURSOR_INPUT);
}

