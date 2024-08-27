#include "warlock.h"
#include "warlock_atom.h"
#include "warlock_error.h"
#include "warlock_builtins.h"

#include <stdio.h>

#define REPL_LEN 256

static bool running = true;
Sexp REPL_Quit(SexpAllocator* alloc, Sexp sexp) {
    running = false;

    return ATOM_MAKE_NIL(alloc);
}

int main(int argc, char** argv) {
    SexpAllocator allocator = SexpAllocator_make();

    Warlock_builtin(&allocator);

    if(argc <= 1) {
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
        String str = { argv[1], strlen(argv[1]) };
        Warlock_run_file(&allocator, str);
    }

    SexpAllocator_free(&allocator);
}

