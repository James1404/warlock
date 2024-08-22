#include "warlock.h"

#include "warlock_atom.h"
#include "warlock_common.h"
#include "warlock_error.h"
#include "warlock_eval.h"
#include "warlock_reader.h"
#include "warlock_builtins.h"
#include "warlock_string.h"

#include <llvm-c/Types.h>
#include <llvm-c/Core.h>

#include <stdio.h>
#include <stdlib.h>

#define INTRINSIC(alloc, name, fn, argc)                                \
    do {                                                                \
    Sexp sexp = ATOM_MAKE_S(alloc, ATOM_INTRINSIC, name, &fn, argc);   \
    SexpAllocator_setLocal(alloc, STR(name), sexp);                     \
    } while(0);

#define GLOBAL(alloc, name, value) SexpAllocator_setLocal(alloc, STR(name), value)
/*
Atom warlock_concat(List args) {
    Atom arg = args.data[0];

    if(arg.ty != ATOM_LIST) return arg;

    return GET_ATOM_LIST(arg).data[GET_ATOM_LIST(arg).length - 1];
}
*/

void Warlock_builtin(SexpAllocator* alloc) {
    INTRINSIC(alloc, "+", Sexp_Add, -1);
    INTRINSIC(alloc, "-", Sexp_Subtract, -1);
    INTRINSIC(alloc, "*", Sexp_Multiply, -1);
    INTRINSIC(alloc, "/", Sexp_Divide, -1);

    INTRINSIC(alloc, "=", Sexp_Equal, 2);
    INTRINSIC(alloc, "!", Sexp_Not, 2);
    INTRINSIC(alloc, ">", Sexp_Greater, 2);
    INTRINSIC(alloc, "<", Sexp_Less, 2);

    INTRINSIC(alloc, "and", Sexp_And, 2);
    INTRINSIC(alloc, "or", Sexp_Or, 2);

    GLOBAL(alloc, "true", ATOM_MAKE_V(alloc, ATOM_BOOLEAN, true));
    GLOBAL(alloc, "false", ATOM_MAKE_V(alloc, ATOM_BOOLEAN, false));

    GLOBAL(alloc, "nil", ATOM_MAKE(alloc, ATOM_NIL));

    INTRINSIC(alloc, "println", Sexp_Println, -1);

    INTRINSIC(alloc, "import", Sexp_Import, 1);

    INTRINSIC(alloc, "eval", Sexp_Eval, 1);

    INTRINSIC(alloc, "first", Sexp_First, 1);
    INTRINSIC(alloc, "rest", Sexp_Rest, 1);
}

void Warlock_run(SexpAllocator* alloc, String src) {
    Reader reader = Reader_make(src);

    Reader_run(&reader, alloc);

    Eval_run(alloc, reader.root);

    Reader_free(&reader);
}

void Warlock_run_file(SexpAllocator* alloc, String path) {
    String nullterminated;
    STR_CPY_ALLOC_NULL(nullterminated, path);
    
    FILE* file = fopen(nullterminated.raw, "r");

    if(!file) {
        Warlock_error("Could not find file '%.*s'", path.len, path.raw);
        return;
    }

    fseek(file, 0, SEEK_END);
    u64 len = ftell(file);

    fseek(file, 0, SEEK_SET);
    char* buffer = malloc(len);

    if(fread(buffer, 1, len, file) != len) {
        Warlock_error("Failed to read file '%.*s'", path.len, path.raw);
        fclose(file);
        return;
    }
    
    fclose(file);

    Warlock_run(alloc, (String) { buffer, len });

    free(buffer);
}
