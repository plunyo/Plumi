#include "commands/lc/lc.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_ENTRIES 1024
#define MAX_NAME_LEN 256
#define MAX_COLS 4

CommandError lc(
    int argc, 
    char** argv, 
    FILE* in __attribute__((unused)), 
    FILE* out __attribute__((unused))
) {
    const char* dir_name = ".";
    int show_all = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            show_all = 1;
        } else {
            dir_name = argv[i];
        }
    }

    DIR* dir = opendir(dir_name);
    if (!dir) {
        perror("opendir failed");
        return CMD_ERR_FAILED;
    }

    char names[MAX_ENTRIES][MAX_NAME_LEN];
    int count = 0;
    int max_len = 0;
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0))
            continue;

        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

        struct stat st;
        int is_dir = 0;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
            is_dir = 1;

        snprintf(names[count], sizeof(names[count]), "%s%s", entry->d_name, is_dir ? "/" : "");
        int len = strlen(names[count]);
        if (len > max_len) max_len = len;

        count++;
        if (count >= MAX_ENTRIES) break;
    }
    closedir(dir);

    if (count == 0) {
        fprintf(out, "(empty)\n");
        return CMD_SUCCESS;
    }

    int cols = count < MAX_COLS ? count : MAX_COLS;
    int rows = (count + cols - 1) / cols;

    fprintf(out, "┌");
    for (int c = 0; c < cols; c++) {
        for (int j = 0; j < max_len + 2; j++) fprintf(out, "─");
        if (c != cols - 1) fprintf(out, "┬");
    }
    fprintf(out, "┐\n");

    for (int r = 0; r < rows; r++) {
        fprintf(out, "│");
        for (int c = 0; c < cols; c++) {
            int idx = r + c * rows;
            if (idx < count)
                fprintf(out, " %-*s │", max_len, names[idx]);
            else
                fprintf(out, " %-*s │", max_len, ""); // empty cell
        }
        fprintf(out, "\n");

        // draw middle border or bottom border
        fprintf(out, "├");
        for (int c = 0; c < cols; c++) {
            for (int j = 0; j < max_len + 2; j++) fprintf(out, "─");
            if (c != cols - 1) fprintf(out, r == rows - 1 ? "┴" : "┼");
        }
        fprintf(out, r == rows - 1 ? "┘\n" : "┤\n");
    }

    return CMD_SUCCESS;
}
