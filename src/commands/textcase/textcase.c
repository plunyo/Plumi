#include "commands/textcase/textcase.h"
#include "commands/commands.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

CommandError textcase(int argc, char** argv, FILE* in, FILE* out) {
    if (argc != 2) {
        fprintf(stderr, "usage: case ( -u | -l )\n");
        return CMD_ERR_INVALID_ARGS;
    }

    int (*func)(int) = NULL;
    if (strcmp(argv[1], "-u") == 0) func = toupper;
    else if (strcmp(argv[1], "-l") == 0) func = tolower;
    else {
        fprintf(stderr, "invalid flag: %s\n", argv[1]);
        return CMD_ERR_INVALID_ARGS;
    }

    int c;
    while ((c = fgetc(in)) != EOF)
        fputc(func(c), out);

    return CMD_SUCCESS;
}