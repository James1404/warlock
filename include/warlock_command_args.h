#ifndef WARLOCK_COMMAND_ARGS_H
#define WARLOCK_COMMAND_ARGS_H

typedef void (*ArgumentParser)(int argc, char **argv);

typedef struct {
    const char* name;
    const char* desc;
    ArgumentParser parser;
} Argument;

#define MAKE_ARGUMENT(name, desc, parser)                                      \
  ((Argument){name, desc, parser})

void ParseArguments(int argc, char** argv, Argument validArgs[], int validArgsLen);

#endif//WARLOCK_COMMAND_ARGS_H
