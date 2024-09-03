#include "warlock.h"
#include "warlock_atom.h"
#include "warlock_error.h"
#include "warlock_command_args.h"

#include <stdio.h>
#include <string.h>

#define REPL_LEN 256

static bool running = true;
static SexpAllocator allocator = { 0 };

Sexp REPL_Quit(SexpAllocator* alloc, Sexp sexp) {
    running = false;
    return ATOM_MAKE_NIL(alloc);
}

void versionParser(int argc, char** argv) {
    printf("%s v%u.%u\n", EXE_NAME, MAJOR_VERSION, MINOR_VERSION);
}

void replParser(int argc, char** argv) {
    INTRINSIC(&allocator, "quit", REPL_Quit, 0);

    while(running) {
        printf("* ");
        char string[REPL_LEN];
        if(!fgets(string, REPL_LEN, stdin)) {
            Warlock_error("fgets failed");
        }

        Sexp result = Warlock_run(&allocator, (String) { string, strlen(string) });

        SexpAllocator_print(&allocator, result);
        printf("\n");
    }
}

void runParser(int argc, char** argv) {
    String str = STR(*argv);
    Warlock_run_file(&allocator, str);
}

#define validArgumentsLen (sizeof(validArguments) / sizeof(*validArguments))
static Argument validArguments[] = {
    MAKE_ARGUMENT("version", "output the version of your installed compiler", &versionParser),
    MAKE_ARGUMENT("run", "run the provided file", &runParser),
    MAKE_ARGUMENT("repl", "run the interactive REPL", &replParser),
};

int main(int argc, char** argv) {
    allocator = SexpAllocator_make();

    Warlock_builtin(&allocator);

    ParseArguments(argc, argv, validArguments, validArgumentsLen);

    SexpAllocator_free(&allocator);
}

