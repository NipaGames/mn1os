#ifndef COMMANDS_H
#define COMMANDS_H

#include "types.h"

enum cmd_return_codes {
    CMD_EXIT_SUCCESS = 0,
    CMD_EXIT_FAILURE = 1,
    CMD_NOT_FOUND = 2,
    CMD_SYNTAX_ERROR = 3,
};

typedef int (*cli_command_handler)(int argc, char* argv[]);
typedef struct {
    char* name;
    cli_command_handler handler;
} cli_command;

const cli_command*  get_cli_commands();
size_t              get_cli_commands_len();

#endif
