#include "commands.h"
#include "rand.h"
#include "pit.h"
#include "keymaps.h"
#include "io.h"
#include "terminal.h"

int c_args(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        t_write(argv[i]);
        t_newline();
    }
    return CMD_EXIT_SUCCESS;
}

int c_clear(int argc, char* argv[]) {
    t_clear();
    return CMD_EXIT_SUCCESS;
}

int c_colors(int argc, char* argv[]) {
    uint8_t original_color;
    t_get_color(&original_color);
    for (uint8_t bg = 0x0; bg <= 0xf; bg++) {
        for (uint8_t fg = 0x0; fg <= 0xf; fg++) {
            t_set_color(vga_entry_color(fg, bg));
            t_write("0x");
            t_write_hex(bg);
            t_write_hex(fg);
            t_set_color(original_color);
        }
        t_newline();
    }
    return CMD_EXIT_SUCCESS;
}

int c_help(int argc, char* argv[]) {
    for (int i = 0; i < get_cli_commands_len(); i++) {
        const cli_command* cmd = &get_cli_commands()[i];
        t_write(cmd->name);
        t_newline();
    }
    return CMD_EXIT_SUCCESS;
}

int c_echo(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        t_write(argv[i]);
        if (i + 1 != argc)
            t_put_char(' ');
    }
    t_newline();
    return CMD_EXIT_SUCCESS;
}

enum flag_type {
    FLAG_TRANS,
    FLAG_BI,
};

const char g_flag_trans_msg[] = "trans rights  <3";
const size_t g_flag_trans_msg_len = sizeof(g_flag_trans_msg) - 1;
const char g_flag_bi_msg[] = ":3";
const size_t g_flag_bi_msg_len = sizeof(g_flag_bi_msg) - 1;

int c_flag(int argc, char* argv[]) {
    if (argc == 1)
        return CMD_EXIT_FAILURE;

    enum flag_type flag;
    if (strcmp(argv[1], "trans") == 0) flag = FLAG_TRANS;
    else if (strcmp(argv[1], "bi") == 0) flag = FLAG_BI;
    else return CMD_EXIT_FAILURE;

    uint8_t original_color;
    t_get_color(&original_color);
    t_hide_cursor();
    int line_h = VGA_HEIGHT / 5;
    switch (flag) {
        case FLAG_TRANS:
            line_h = VGA_HEIGHT / 5;
            t_fill(0 * line_h * VGA_WIDTH, line_h * VGA_WIDTH,      vga_entry(' ', 0xbf));
            t_fill(1 * line_h * VGA_WIDTH, line_h * VGA_WIDTH,      vga_entry(' ', 0xdf));
            t_fill(2 * line_h * VGA_WIDTH, line_h * VGA_WIDTH,      vga_entry(' ', 0xf0));
            t_fill(3 * line_h * VGA_WIDTH, line_h * VGA_WIDTH,      vga_entry(' ', 0xdf));
            t_fill(4 * line_h * VGA_WIDTH, line_h * VGA_WIDTH,      vga_entry(' ', 0xbf));
            t_set_color(0xf0);
            t_set_pos(VGA_WIDTH / 2 - g_flag_trans_msg_len / 2, VGA_HEIGHT / 2);
            t_write_s(g_flag_trans_msg, g_flag_trans_msg_len);
            break;
        case FLAG_BI:
            line_h = VGA_HEIGHT / 5;
            t_fill(0 * line_h * VGA_WIDTH, line_h * 2 * VGA_WIDTH,  vga_entry(' ', 0xcf));
            t_fill(2 * line_h * VGA_WIDTH, line_h * VGA_WIDTH,      vga_entry(' ', 0xdf));
            t_fill(3 * line_h * VGA_WIDTH, line_h * 2 * VGA_WIDTH,  vga_entry(' ', 0x9f));
            t_set_color(0xdf);
            t_set_pos(VGA_WIDTH / 2 - g_flag_bi_msg_len / 2, VGA_HEIGHT / 2);
            t_write_s(g_flag_bi_msg, g_flag_bi_msg_len);
            break;
    }
    t_set_color(original_color);
    t_wait_for_input();
    t_clear();
    t_sync_cursor_pos();
    t_set_cursor(TEXT_MODE_CURSOR_UNDERLINE);
    return CMD_EXIT_SUCCESS;
}

int c_keys(int argc, char* argv[]) {
    if (argc == 1) {
        switch (kb_get_keymap()) {
            case KEYMAP_US:
                t_write("us");
                break;
            case KEYMAP_FI:
                t_write("fi");
                break;
        }
        t_newline();
    }
    else {
        if (strcmp(argv[1], "us") == 0)
            kb_set_keymap(KEYMAP_US);
        else if (strcmp(argv[1], "fi") == 0)
            kb_set_keymap(KEYMAP_FI);
        else
            return CMD_EXIT_FAILURE;
    }
    return CMD_EXIT_SUCCESS;
}

int c_mn1(int argc, char* argv[]) {
    int n;
    while (1) {
        t_write("Anna arvattava kokonaisluku väliltä 1-100: ");
        int err = 0;
        n = str_to_int_dec(t_scan_line(), &err);
        if (!err && n >= 1 && n <= 100)
            break;
    }
    int guess = 0;
    int i = 0;
    srand(pit_get_ticks());
    while (guess != n) {
        guess = rand() % 100 + 1;
        i++;
    }
    t_write("Arvauksia tarvittiin ");
    t_write_dec(i);
    t_write(" kpl.\n");
    return CMD_EXIT_SUCCESS;
}

int c_reboot(int argc, char* argv[]) {
    reboot();
    return CMD_EXIT_SUCCESS;
}

const cli_command g_cli_commands[] = {
    { "args", c_args },
    { "clear", c_clear },
    { "colors", c_colors },
    { "echo", c_echo },
    { "flag", c_flag },
    { "help", c_help },
    { "keys", c_keys },
    { "mn1", c_mn1 },
    { "reboot", c_reboot },
};

const cli_command* get_cli_commands() {
    return g_cli_commands;
}

size_t get_cli_commands_len() {
    return sizeof(g_cli_commands) / sizeof(g_cli_commands[0]);
}
