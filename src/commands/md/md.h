#pragma once

#include "commands/commands.h"

CommandError md(
    int argc __attribute__((unused)), 
    char** argv __attribute__((unused)), 
    FILE* in __attribute__((unused)), 
    FILE* out __attribute__((unused))
);
