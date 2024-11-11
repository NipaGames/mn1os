#include "cli.h"
#include "terminal.h"
#include "rand.h"
#include "pit.h"
#include "keymaps.h"
#include "io.h"

int c_args(int argc, char* argv[]);
int c_clear(int argc, char* argv[]);
int c_echo(int argc, char* argv[]);
int c_help(int argc, char* argv[]);
int c_keys(int argc, char* argv[]);
int c_mn1(int argc, char* argv[]);
int c_reboot(int argc, char* argv[]);

const cli_command g_cli_commands[] = {
    { "args", c_args },
    { "clear", c_clear },
    { "echo", c_echo },
    { "help", c_help },
    { "keys", c_keys },
    { "mn1", c_mn1 },
    { "reboot", c_reboot },
};

void enter_cli() {
    while (1) {
        t_write("> ");
        char* line = t_scan_line();
        int begin = 0;
        int i = 0;
        int argc = 1;
        int quotes = 0;
        while (isspace(line[begin]))
            begin++;
        if (line[begin] == '\0')
            continue;
        i = begin;
        while (line[i] != '\0') {
            if (isspace(line[i]) && (quotes % 2) == 0) {
                while (isspace(line[++i]));
                if (line[i] == '\0')
                    break;
                argc++;
            }
            else if (line[i++] == '"')
                quotes++;
        }
        if ((quotes % 2) == 1) {
            t_write("syntax error\n");
            continue;
        }
        char* argv[argc];
        i = begin;
        argc = 0;
        quotes = 0;
        while (line[i] != '\0') {
            if (line[i] == '"') {
                i++;
                quotes++;
                continue;
            }
            argv[argc++] = &line[i++];
            int inquotes = ((quotes % 2) == 1);
            while ((!isspace(line[i]) || inquotes) && line[i] != '\0') {
                if (line[i] == '"') {
                    quotes++;
                    break;
                }
                i++;
            }
            if (line[i] == '\0')
                break;
            line[i] = '\0';
            if ((quotes % 2) == 1) {
                i++;
                continue;
            }
            while (isspace(line[++i]));
        }
        for (size_t i = 0; i < sizeof(g_cli_commands) / sizeof(g_cli_commands[0]); i++) {
            const cli_command* cmd = &g_cli_commands[i];
            if (strcmp(argv[0], cmd->name) == 0) {
                int err = cmd->handler(argc, argv);
                if (err != CMD_EXIT_SUCCESS) {
                    t_write("error code ");
                    t_write_dec(err);
                    t_newline();
                }
                break;
            }
        }
    }
}

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

int c_help(int argc, char* argv[]) {
    for (int i = 0; i < sizeof(g_cli_commands) / sizeof(g_cli_commands[0]); i++) {
        const cli_command* cmd = &g_cli_commands[i];
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

