#include "commands/cd/cd.h"
#include "commands/commands.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

CommandError cd(
    int argc __attribute__((unused)),
    char** argv __attribute__((unused)),
    FILE* in __attribute__((unused)),
    FILE* out __attribute__((unused))
) {
    if (argc != 2) {
        RED_ERR("usage: cd {PATH}\n");
        return CMD_ERR_INVALID_ARGS;
    }

    if (chdir(argv[1]) != 0) {
        switch(errno) {
        case ENOENT:
            RED_ERR("cd: no file or directory brochacho: %s\n", argv[1]);
            return CMD_ERR_FILE_NOT_FOUND;
        case ENOTDIR:
            RED_ERR("cd: schizo ass, ts NOT a directory: %s\n", argv[1]);
            return CMD_ERR_INVALID_ARGS;
        case EACCES:
            RED_ERR("cd: permission denied: %s\n", argv[1]);
            return CMD_ERR_PERMISSION;
        default:
            perror("cd failed");
            return CMD_ERR_FAILED;
        }
    }

    return CMD_SUCCESS;
}