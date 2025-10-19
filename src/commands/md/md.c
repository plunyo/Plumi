#include "commands/md/md.h"
#include "commands/commands.h"
#include <sys/stat.h>
#include <unistd.h>

CommandError md(
    int argc __attribute__((unused)), 
    char** argv __attribute__((unused)), 
    FILE* in __attribute__((unused)), 
    FILE* out __attribute__((unused))
) {
    if (argc != 2) {
        RED_ERR("usage: md [path/]name\n");
        return CMD_ERR_INVALID_ARGS;
    }

    if (mkdir(argv[1], 0777) != 0) {
        perror("error creating directory");
    }

    return CMD_SUCCESS;
}