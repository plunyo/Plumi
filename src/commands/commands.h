#pragma once

#include <stdio.h>

#define MAX_COMMANDS 1024

typedef enum {
    CMD_SUCCESS,
    CMD_ERR_UNKNOWN,
    CMD_ERR_INVALID_ARGS,
    CMD_ERR_FILE_NOT_FOUND,
    CMD_ERR_PERMISSION,
} CommandError;

typedef CommandError(*Command)(int argc, char** argv, FILE* in, FILE* out);

typedef struct {
    const char* name;
    Command func;
} CommandEntry;

extern CommandEntry commands[MAX_COMMANDS];
extern int command_count;

void register_command(char* name, Command func);
void init_commands();
CommandError run_command(char* name, int argc, char** argv, FILE* in, FILE* out);
