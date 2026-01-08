#include "warlock_command_args.h"

#include <stdio.h>
#include <string.h>

void ParseArguments(int argc, char** argv, Argument validArgs[],
                    int validArgsLen) {
    argc--;
    argv++;

    if (argc <= 0) {
        printf("Expected arguments\n");

        return;
    }

    if (strcmp(*argv, "help") == 0) {
        for (int i = 0; i < validArgsLen; i++) {
            Argument arg = validArgs[i];
            printf("%s :: %s\n", arg.name, arg.desc);
        }

        return;
    }

    for (int i = 0; i < validArgsLen; i++) {
        Argument* arg = validArgs + i;

        if (strcmp(arg->name, *argv) == 0) {
            arg->parser(argc - 1, argv + 1);
            break;
        }
    }
}
