// commands.c
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "commands/commands.h"

// include your internal commands
#include "commands/print/print.h"
#include "commands/textcase/textcase.h"
#include "commands/clear/clear.h"
#include "commands/cd/cd.h"
#include "commands/lc/lc.h"
#include "commands/md/md.h"
#include "commands/rm/rm.h"
#include "commands/mf/mf.h"

CommandEntry commands[MAX_COMMANDS];
int command_count = 0;

void register_command(char* name, Command func) {
    if (command_count >= MAX_COMMANDS) return;
    commands[command_count].name = name;
    commands[command_count].func = func;
    command_count++;
}

void init_commands() {
    register_command("print", print);
    register_command("textcase", textcase);
    register_command("clear", clear);
    register_command("cd", cd);
    register_command("lc", lc);
    register_command("md", md);
    register_command("rm", rm);
    register_command("mf", mf);
}

int run_external(const char *path, char *const argv[], FILE* in, FILE* out) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed.");
        return -1;
    } else if (pid == 0) {
        if (in != NULL && fileno(in) != STDIN_FILENO) {
            dup2(fileno(in), STDIN_FILENO);
        }
        if (out != NULL && fileno(out) != STDOUT_FILENO) {
            dup2(fileno(out), STDOUT_FILENO);
        }

        execv(path, argv);
        perror("execv failed.");
        exit(1);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed.");
            return -1;
        }

        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        else
            return -1;
    }
}

CommandError run_command(char* name, int argc, char** argv, FILE* in, FILE* out) {
    // check internal commands first
    for (int i = 0; i < command_count; i++) 
        if (strcmp(name, commands[i].name) == 0) 
            return commands[i].func(argc, argv, in, out);

    if (name[0] == '!') {
        char path[512];
        snprintf(path, sizeof(path), "/usr/bin/%s", name + 1);

        if (access(path, F_OK | X_OK) != 0) {
            fprintf(stderr, "external command not found or not executable: %s\n", path);
            return CMD_ERR_UNKNOWN;
        }

        char *exec_argv[argc + 1];
        exec_argv[0] = path;
        for (int i = 1; i < argc; i++) {
            exec_argv[i] = argv[i];
        }
        exec_argv[argc] = NULL;

        int code = run_external(path, exec_argv, in, out);
        return (code == 0) ? CMD_SUCCESS : CMD_ERR_FAILED;
    }

    fprintf(stderr, "command not found: %s\n", name);
    return CMD_ERR_UNKNOWN;
}
