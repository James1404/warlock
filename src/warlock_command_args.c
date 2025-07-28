#include "warlock_command_args.h"
#include "warlock_string.h"

#include <stdio.h>

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
            printf("%.*s :: %.*s\n", arg.name.len, arg.name.raw, arg.desc.len,
                   arg.desc.raw);
        }

        return;
    }

    for (int i = 0; i < validArgsLen; i++) {
        Argument* arg = validArgs + i;

        if (String_cmp_with_raw(arg->name, *argv)) {
            arg->parser(argc - 1, argv + 1);
            break;
        }
    }
}
