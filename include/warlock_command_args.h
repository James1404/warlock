#ifndef WARLOCK_COMMAND_ARGS_H
#define WARLOCK_COMMAND_ARGS_H

#include "warlock_string.h"

typedef void (*ArgumentParser)(int argc, char **argv);

typedef struct {
    String name, desc;
    ArgumentParser parser;
} Argument;

#define MAKE_ARGUMENT(name, desc, parser)                                      \
  ((Argument){STR(name), STR(desc), parser})

void ParseArguments(int argc, char** argv, Argument validArgs[], int validArgsLen);

#endif//WARLOCK_COMMAND_ARGS_H
