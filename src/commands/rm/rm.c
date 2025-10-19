#include "commands/commands.h"
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

CommandError force_remove(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        if (errno == ENOENT)
            return CMD_ERR_FILE_NOT_FOUND;
        else if (errno == EACCES || errno == EPERM)
            return CMD_ERR_PERMISSION;
        else
            return CMD_ERR_FAILED;
    }

    if (S_ISDIR(st.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir) {
            if (errno == EACCES || errno == EPERM)
                return CMD_ERR_PERMISSION;
            else
                return CMD_ERR_FAILED;
        }

        struct dirent *entry;
        char full_path[512];

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            CommandError err = force_remove(full_path);
            if (err != CMD_SUCCESS) {
                closedir(dir);
                return err; // propagate first error
            }
        }

        closedir(dir);
        if (rmdir(path) != 0) {
            if (errno == EACCES || errno == EPERM)
                return CMD_ERR_PERMISSION;
            else
                return CMD_ERR_FAILED;
        }
    } else {
        if (unlink(path) != 0) {
            if (errno == ENOENT)
                return CMD_ERR_FILE_NOT_FOUND;
            else if (errno == EACCES || errno == EPERM)
                return CMD_ERR_PERMISSION;
            else
                return CMD_ERR_FAILED;
        }
    }

    return CMD_SUCCESS;
}

CommandError rm(
    int argc,
    char **argv,
    FILE *in __attribute__((unused)),
    FILE *out __attribute__((unused))
) {
    if (argc < 2) {
        fprintf(stderr, "usage: rm <file_or_dir>...\n");
        return CMD_ERR_INVALID_ARGS;
    }

    CommandError ret = CMD_SUCCESS;
    for (int i = 1; i < argc; i++) {
        CommandError err = force_remove(argv[i]);
        if (err != CMD_SUCCESS) {
            fprintf(stderr, "rm: failed to remove '%s'\n", argv[i]);
            ret = err;
        }
    }

    return ret;
}
