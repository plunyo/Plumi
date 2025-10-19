#include "commands/mf/mf.h"
#include "commands/commands.h"

#include <stdio.h>

CommandError mf(
    int argc __attribute__((unused)), 
    char** argv __attribute__((unused)), 
    FILE* in __attribute__((unused)), 
    FILE* out __attribute__((unused))
) {
    if (argc != 2) {
        RED_ERR("usage: mf <name>\n");
        return CMD_ERR_INVALID_ARGS;
    }

    FILE* file = fopen(argv[1], "w");
    if (!file) {
        perror("couldn't make file");
        return CMD_ERR_FAILED;
    }

    fclose(file);
    printf("file created: %s\n", argv[1]);
    return CMD_SUCCESS;
}