#ifndef CLI_H
#define CLI_H

#define CMD_EXIT_SUCCESS 0
#define CMD_EXIT_FAILURE 1

typedef int (*cli_command_handler)(int argc, char* argv[]);

typedef struct {
    const char* name;
    cli_command_handler handler;
} cli_command;

void enter_cli();

#endif
