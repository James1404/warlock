#include "warlock.h"
#include "warlock_atom.h"
#include "warlock_error.h"

#include <stdio.h>
#include <string.h>

#define REPL_LEN 256

#define EXE_NAME "Warlock"
#define MAJOR_VERSION 0
#define MINOR_VERSION 01

static bool running = true;
Sexp REPL_Quit(SexpAllocator* alloc, Sexp sexp) {
    running = false;
    printf("quitted");

    return ATOM_MAKE_NIL(alloc);
}

int main(int argc, char** argv) {
    SexpAllocator allocator = SexpAllocator_make();

    Warlock_builtin(&allocator);

    argc--;
    argv++;

    if(argc <= 0) {
        INTRINSIC(&allocator, "quit", REPL_Quit, 0);

        while(running) {
            printf("* ");
            char string[REPL_LEN];
            if(!fgets(string, REPL_LEN, stdin)) {
                Warlock_error("fgets failed");
            }

            Sexp result = Warlock_run(&allocator, (String) { string,
                                                             strlen(string) });

            SexpAllocator_print(&allocator, result);
            printf("\n");
        }
    }
    else {
        if(strcmp(*argv, "version") == 0) {
            printf("%s v%u.%u\n", EXE_NAME, MAJOR_VERSION, MINOR_VERSION);
        }
        else if(strcmp(*argv, "help") == 0) {
            printf("%s v%u.%u :: HELP\n", EXE_NAME, MAJOR_VERSION, MINOR_VERSION);
        }
        else if(strcmp(*argv, "run") == 0) {
            String str = STR(argv[1]);
            Warlock_run_file(&allocator, str);
        }
    }

    SexpAllocator_free(&allocator);
}

