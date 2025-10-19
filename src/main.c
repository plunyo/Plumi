// main.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "commands/commands.h"

#define MAX_HOSTNAME_LEN 1024
#define MAX_CWD_LEN 2048
#define MAX_LINE 1024
#define MAX_ARGS 128
#define MAX_PIPE 32

// ansi color/style codes
#define COLOR_USERHOST "\033[32m"   //\\ green
#define COLOR_CWD      "\033[36m"  //  \\ cyan
#define COLOR_DOLLAR   "\033[1m"  //   // bold
#define COLOR_RESET    "\033[0m" //   // reset all styles/colors

void print_prompt(const char* cwd, const char* user, const char* hostname) {
    printf(
        COLOR_USERHOST "%s@%s" 
        ":"
        COLOR_CWD "%s"      
        COLOR_DOLLAR "$ " COLOR_RESET,
        user, hostname, cwd
    );
}

int split_pipes(char* line, char* cmds[MAX_PIPE]) {
    int count = 0;
    char* token = strtok(line, "|");
    while (token && count < MAX_PIPE) { 
        cmds[count++] = token;
        token = strtok(NULL, "|");
    }
    return count;
}

int tokenize_cmd(char* cmd, char* argv[MAX_ARGS]) {
    int argc = 0;
    char* token = strtok(cmd, " \t");
    while (token && argc < MAX_ARGS - 1) {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }
    argv[argc] = NULL;
    return argc;
}

int main() {
    fprintf(stderr, "\033[31m");
    char* user = getenv("USER");
    char hostname[MAX_HOSTNAME_LEN];
    if (gethostname(hostname, MAX_HOSTNAME_LEN) != 0) { perror("gethostname"); return 1; }

    init_commands();

    while (true) {
        char cwd[MAX_CWD_LEN];
        if (getcwd(cwd, MAX_CWD_LEN) == NULL) {
            perror("getcwd");
            return EXIT_FAILURE;
        }

        print_prompt(cwd, user, hostname);

        char line[MAX_LINE];
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        char* cmds[MAX_PIPE];
        int pipe_count = split_pipes(line, cmds);

        FILE* in = stdin;
        FILE* tmp = NULL;

        for (int i = 0; i < pipe_count; i++) {
            char* argv[MAX_ARGS];
            int argc = tokenize_cmd(cmds[i], argv);
            if (argc == 0) continue;

            if (strcmp(argv[0], "exit") == 0)
                return EXIT_SUCCESS;

            FILE* out = (i < pipe_count - 1) ? tmpfile() : stdout;
            if (!out) { perror("tmpfile"); break; }

            CommandError error = run_command(argv[0], argc, argv, in, out);
            if (error == CMD_ERR_UNKNOWN) {
                fprintf(stderr, "command '%s' failed with code %d\n", argv[0], error);
                break;
            }

            if (i < pipe_count - 1) {
                fflush(out);
                fseek(out, 0, SEEK_SET);
                in = out;
                tmp = out;
            }
        }

        if (tmp) fclose(tmp);
    }

    return EXIT_SUCCESS;
}
