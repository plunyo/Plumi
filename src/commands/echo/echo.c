#include "commands/echo/echo.h"
#include "commands/commands.h"
#include <stdio.h>

CommandError echo(int argc, char** argv, FILE* in __attribute__((unused)), FILE* out) {
    for (int i = 1; i < argc; i++) {
        fprintf(out, "%s ", argv[i]);
    }
    fprintf(out, "\n");
        
    return CMD_SUCCESS;
}