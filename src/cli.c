#include "cli.h"
#include "terminal.h"
#include "commands.h"

int count_args(char* c) {
    int argc = 0;
    while (*c != '\0') {
        while (isspace(*c)) c++;
        if (*c == '"') {
            argc++;
            while (*++c != '"')
                if (*c == '\0') return -1;
            c++;
        }
        else if (*c != '\0') {
            argc++;
            while (*++c != '\0') {
                if (*c == '"') break;
                if (isspace(*c)) break;
            }
            if (*c == '"') continue;
        }
    }
    return argc;
}

void split_args(char* c, char* argv[]) {
    int argc = 0;
    while (*c != '\0') {
        while (isspace(*c)) c++;
        if (*c == '"') {
            *c = '\0';
            argv[argc++] = ++c;
            while (*c != '"') c++;
        }
        else {
            argv[argc++] = c;
            while (!isspace(*++c)) {
                if (*c == '"') break;
                else if (*c == '\0') return;
            }
            if (*c == '"') continue;
        }
        *c++ = '\0';
    }
}

int exec_command(char* input) {
    int argc = count_args(input);
    if (argc == 0) return 0;
    else if (argc == -1) return CMD_SYNTAX_ERROR;
    char* argv[argc];
    split_args(input, argv);
    for (int i = 0; i < get_cli_commands_len(); i++) {
        const cli_command* cmd = &get_cli_commands()[i];
        if (strcmp(argv[0], cmd->name) == 0)
            return cmd->handler(argc, argv);
    }
    return CMD_NOT_FOUND;
}

void enter_cli() {
    int err = 0;
    while (1) {
        uint8_t original_color;
        t_get_color(&original_color);
        t_set_color(err ? 0x0c : 0x0a);
        t_write("> ");
        t_set_color(original_color);
        char* line = t_scan_line();
        err = exec_command(line);
        if (err != CMD_EXIT_SUCCESS) {
            switch (err) {
                case CMD_NOT_FOUND:
                    break;
                case CMD_SYNTAX_ERROR:
                    t_write("syntax error\n");
                    break;
                default:
                    t_write("error code ");
                    t_write_dec(err);
                    t_newline();
                    break;
            }
        }
    }
}

