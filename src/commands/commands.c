#include <string.h>
#include "commands/commands.h"
#include "commands/echo/echo.h"
#include "commands/textcase/textcase.h"

CommandEntry commands[MAX_COMMANDS];

int command_count = 0;

void register_command(char* name, Command func) {
    if (command_count >= MAX_COMMANDS) return; // optional safety check
    commands[command_count].name = name;
    commands[command_count].func = func;
    command_count++;
}

void init_commands() {
    register_command("echo", echo);
    register_command("textcase", textcase);
}

CommandError run_command(char* name, int argc, char** argv, FILE* in, FILE* out) {
    for (int i = 0; i < command_count; i++) 
        if (strcmp(name, commands[i].name) == 0) 
            return commands[i].func(argc, argv, in, out);

    fprintf(stderr, "command not found: %s\n", name);
    return CMD_ERR_UNKNOWN;
}
