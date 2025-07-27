#include "warlock.h"

#include "warlock_atom.h"
#include "warlock_common.h"
#include "warlock_error.h"
#include "warlock_eval.h"
#include "warlock_reader.h"
#include "warlock_builtins.h"
#include "warlock_string.h"

#include <stdio.h>
#include <stdlib.h>

void Warlock_builtin(Environment* env) {
    INTRINSIC(env, "def", Sexp_Def, -1, false);
    INTRINSIC(env, "fn", Sexp_Fn, -1, false);
    INTRINSIC(env, "quote", Sexp_Quote, -1, false);

    INTRINSIC(env, "print_env", Sexp_PrintEnv, 0, true);

    INTRINSIC(env, "+", Sexp_Add, -1, true);
    INTRINSIC(env, "-", Sexp_Subtract, -1, true);
    INTRINSIC(env, "*", Sexp_Multiply, -1, true);
    INTRINSIC(env, "/", Sexp_Divide, -1, true);

    INTRINSIC(env, "=", Sexp_Equal, 2, true);
    INTRINSIC(env, "!", Sexp_Not, 2, true);
    INTRINSIC(env, ">", Sexp_Greater, 2, true);
    INTRINSIC(env, "<", Sexp_Less, 2, true);

    INTRINSIC(env, "and", Sexp_And, 2, true);
    INTRINSIC(env, "or", Sexp_Or, 2, true);

    GLOBAL(env, "true", ATOM_MAKE_V(env, ATOM_BOOLEAN, true));
    GLOBAL(env, "false", ATOM_MAKE_V(env, ATOM_BOOLEAN, false));

    GLOBAL(env, "nil", ATOM_MAKE(env, ATOM_NIL));

    INTRINSIC(env, "println", Sexp_Println, -1, true);

    INTRINSIC(env, "import", Sexp_Import, 1, true);

    INTRINSIC(env, "eval", Sexp_Eval, 1, true);

    INTRINSIC(env, "first", Sexp_First, 1, true);
    INTRINSIC(env, "rest", Sexp_Rest, 1, true);

    INTRINSIC(env, "if", Sexp_If, 3, true);
}

Sexp Warlock_run(Environment* env, String src) {
    Reader reader = Reader_make(src);

    Reader_run(&reader, env);

    if(Warlock_foundError()) return ATOM_MAKE_NIL(env);

    Sexp result = ATOM_MAKE_NIL(env);

    for (i32 i = 0; i < reader.linesLen; i++) {
        Sexp line = reader.lines[i];
        result = Eval_run(env, line);
    }

    if(Warlock_foundError()) return ATOM_MAKE_NIL(env);

    Reader_free(&reader);

    return result;
}

Sexp Warlock_run_file(Environment* env, String path) {
    String nullterminated = String_copy_null(path);
    
    FILE* file = fopen(nullterminated.raw, "r");

    if(!file) {
        Warlock_error("Could not find file '%.*s'", path.len, path.raw);
        fclose(file);
        String_free(&nullterminated);
        return ATOM_MAKE_NIL(env);
    }

    fseek(file, 0, SEEK_END);
    u64 len = ftell(file);

    fseek(file, 0, SEEK_SET);
    char* buffer = malloc(len);

    if(fread(buffer, 1, len, file) != len) {
        Warlock_error("Failed to read file '%.*s'", path.len, path.raw);
        fclose(file);
        return ATOM_MAKE_NIL(env);
    }
    
    fclose(file);

    Sexp result = Warlock_run(env, (String) { buffer, len });

    free(buffer);
    String_free(&nullterminated);

    return result;
}

