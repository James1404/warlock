#include "warlock.h"
#include "warlock_atom.h"
#include "warlock_command_args.h"
#include "warlock_error.h"
#include "warlock_string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <editline.h>

static bool running = true;
static Environment environment = {0};

Sexp REPL_Quit(Environment* env, Sexp sexp) {
    running = false;
    return ATOM_MAKE_NIL(env);
}

void versionParser(int argc, char** argv) {
    printf("%s v%u.%u\n", EXE_NAME, MAJOR_VERSION, MINOR_VERSION);
}

void replParser(int argc, char** argv) {
    INTRINSIC(&environment, "quit", REPL_Quit, 0, false);

    while (running) {
        char* input = readline("warlock> ");
        add_history(input);

        Sexp result = Warlock_run(&environment, (String){input, strlen(input)});

        Environment_print(&environment, result);
        printf("\n");

        free(input);
    }
}

#ifdef __linux__
#include <dirent.h>
#endif

#ifdef __linux__
static int parse_ext(const struct dirent* dir) {
    if (!dir) {
        return 0;
    }

    if (dir->d_type == DT_REG) {
        const char* ext = strchr(dir->d_name, '.');
        if ((!ext) || (ext == dir->d_name)) {
            return 0;
        } else {
            if (strcmp(ext, ".mr") == 0) {
                return 1;
            }
        }
    }

    return 0;
}
#endif
void testParser(int argc, char** argv) {
#ifdef __linux__
    struct dirent** namelist;
    int n = scandir("./test", &namelist, parse_ext, alphasort);
    if (n == -1) {
        perror("scandir");
    }

    while (n--) {
        String str = STR(namelist[n]->d_name);
        Warlock_run_file(&environment, str);
        free(namelist[n]);
    }

    free(namelist);
#endif
}

void runParser(int argc, char** argv) {
    String str = STR(*argv);
    Warlock_run_file(&environment, str);
}

int main(int argc, char** argv) {
    environment = Environment_make();

    Warlock_builtin(&environment);

    Argument validArguments[] = {
        MAKE_ARGUMENT("version",
                      "output the version of your installed compiler",
                      &versionParser),
        MAKE_ARGUMENT("run", "run the provided file", &runParser),
        MAKE_ARGUMENT("repl", "run the interactive REPL", &replParser),
        MAKE_ARGUMENT("test", "run all the test files", &testParser),
    };
    usize validArgumentsLen = sizeof(validArguments) / sizeof(*validArguments);

    ParseArguments(argc, argv, validArguments, validArgumentsLen);

    Environment_free(&environment);
}
