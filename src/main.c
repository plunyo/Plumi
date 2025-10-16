// main.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "commands/commands.h"

#define MAX_LINE 1024
#define MAX_ARGS 128
#define MAX_PIPE 32

int main() {
    init_commands();

    while (true) {
        printf("$ ");
        char line[MAX_LINE];
        if (!fgets(line, sizeof(line), stdin)) break;

        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        char *cmds[MAX_PIPE];
        int pipe_count = 0;

        char *pipe_token = strtok(line, "|");
        while (pipe_token && pipe_count < MAX_PIPE) {
            cmds[pipe_count++] = pipe_token;
            pipe_token = strtok(NULL, "|");
        }

        FILE *in = stdin;
        FILE *tmp = NULL;

        for (int i = 0; i < pipe_count; i++) {
            // tokenize command arguments
            char *argv[MAX_ARGS];
            int argc = 0;

            char *token = strtok(cmds[i], " \t");
            while (token && argc < MAX_ARGS - 1) {
                argv[argc++] = token;
                token = strtok(NULL, " \t");
            }
            argv[argc] = NULL;

            if (argc == 0) continue;

            if (strcmp(argv[0], "exit") == 0) return EXIT_SUCCESS;

            // pipe for next command
            FILE *out = stdout;
            if (i < pipe_count - 1) {
                tmp = tmpfile();
                if (!tmp) { perror("tmpfile"); break; }
                out = tmp;
            }

            CommandError error = run_command(argv[0], argc, argv, in, out);
            if (error != CMD_SUCCESS) {
                fprintf(stderr, "command '%s' failed with code %d\n", argv[0], error);
                break;
            }

            // prepare input for next command
            if (i < pipe_count - 1) {
                fflush(tmp);
                fseek(tmp, 0, SEEK_SET);
                in = tmp;
            }
        }

        if (tmp) fclose(tmp); // cleanup last temp file
    }

    return EXIT_SUCCESS;
}
