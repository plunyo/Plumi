#include "commands/clear/clear.h"
#include "commands/commands.h"
#include <stdio.h>

CommandError clear(int argc __attribute__((unused)), char** argv __attribute__((unused)), FILE* in __attribute__((unused)), FILE* out __attribute__((unused))) {
    printf("\033c");
    fflush(stdout);
    return CMD_SUCCESS;
}