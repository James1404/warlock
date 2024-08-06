#include "scheme.h"

#include "scheme_allocator.h"
#include "scheme_atom.h"
#include "scheme_common.h"
#include "scheme_env.h"
#include "scheme_error.h"
#include "scheme_eval.h"
#include "scheme_printer.h"
#include "scheme_reader.h"

#include <llvm-c/Types.h>
#include <llvm-c/Core.h>

#include <stdio.h>
#include <stdlib.h>

void Scheme_init(void) {
    MainAllocator_init();
    Env_init();
}

void Scheme_free(void) {
    Env_free();
    MainAllocator_free();
}

#define INTRINSIC(name, fn, argc)\
    do {\
        Atom atom = ATOM_INTRINSIC(name, &fn, argc);\
        Env_set(String_make(name), atom);\
    } while(0);

#define GLOBAL(name, value) Env_set(String_make(name), value)

Atom scheme_add(List args) {
    Atom result = ATOM_INTEGER(0);

    for(u64 i = 0; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) += GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom scheme_sub(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) -= GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom scheme_mul(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) *= GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom scheme_div(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) /= GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom scheme_eq(List args) {
    Atom lhs = args.data[0];
    Atom rhs = args.data[1];

    if(lhs.ty != rhs.ty) {
        Scheme_error("Arguments must be the same type");
        return ATOM_NIL();
    }

    switch(lhs.ty) {
        case ATOM_INTEGER: return ATOM_BOOLEAN(GET_ATOM_INTEGER(lhs) == GET_ATOM_INTEGER(rhs));
        case ATOM_REAL: return ATOM_BOOLEAN(GET_ATOM_REAL(lhs) == GET_ATOM_REAL(rhs));
        case ATOM_BOOLEAN: return ATOM_BOOLEAN(GET_ATOM_BOOLEAN(lhs) == GET_ATOM_BOOLEAN(rhs));
        case ATOM_STRING: return ATOM_BOOLEAN(String_cmp(GET_ATOM_STRING(lhs), GET_ATOM_STRING(rhs)));
        case ATOM_SYMBOL: return ATOM_BOOLEAN(String_cmp(GET_ATOM_STRING(lhs), GET_ATOM_STRING(rhs)));
        case ATOM_KEYWORD: return ATOM_BOOLEAN(String_cmp(GET_ATOM_STRING(lhs), GET_ATOM_STRING(rhs)));
        default: return ATOM_BOOLEAN(false);
    }

    return ATOM_BOOLEAN(false);
}

Atom scheme_not(List args) {
    Atom arg = args.data[0];

    if(arg.ty != ATOM_BOOLEAN) {
        Scheme_error("Arguments must be a boolean");
        return ATOM_NIL();
    }

    return ATOM_BOOLEAN(!GET_ATOM_BOOLEAN(arg));
}

Atom scheme_greater(List args) {
    Atom lhs = args.data[0];
    Atom rhs = args.data[1];

    if(lhs.ty != rhs.ty) {
        Scheme_error("Arguments must be the same type");
        return ATOM_NIL();
    }

    switch(lhs.ty) {
        case ATOM_INTEGER: return ATOM_BOOLEAN(GET_ATOM_INTEGER(lhs) > GET_ATOM_INTEGER(rhs));
        case ATOM_REAL: return ATOM_BOOLEAN(GET_ATOM_REAL(lhs) > GET_ATOM_REAL(rhs));
        default:
           Scheme_error("Invalid comparison types"); 
           return ATOM_NIL();
    }

    return ATOM_BOOLEAN(false);
}

Atom scheme_lesser(List args) {
    Atom lhs = args.data[0];
    Atom rhs = args.data[1];

    if(lhs.ty != rhs.ty) {
        Scheme_error("Arguments must be the same type");
        return ATOM_NIL();
    }

    switch(lhs.ty) {
        case ATOM_INTEGER: return ATOM_BOOLEAN(GET_ATOM_INTEGER(lhs) < GET_ATOM_INTEGER(rhs));
        case ATOM_REAL: return ATOM_BOOLEAN(GET_ATOM_REAL(lhs) < GET_ATOM_REAL(rhs));
        default:
           Scheme_error("Invalid comparison types"); 
           return ATOM_NIL();
    }

    return ATOM_BOOLEAN(false);
}

Atom scheme_and(List args) {
    for(u64 i = 0; i < args.length; i++) {
        if(!GET_ATOM_BOOLEAN(args.data[i])) {
            return ATOM_BOOLEAN(false);
        }
    }

    return ATOM_BOOLEAN(true);
}

Atom scheme_or(List args) {
    for(u64 i = 0; i < args.length; i++) {
        if(GET_ATOM_BOOLEAN(args.data[i])) {
            return ATOM_BOOLEAN(true);
        }
    }

    return ATOM_BOOLEAN(false);
}

Atom scheme_head(List args) {
    Atom arg = args.data[0];

    if(arg.ty != ATOM_LIST) return arg;

    return GET_ATOM_LIST(arg).data[0];
}

Atom scheme_tail(List args) {
    Atom arg = args.data[0];

    if(arg.ty != ATOM_LIST) return arg;

    return GET_ATOM_LIST(arg).data[GET_ATOM_LIST(arg).length - 1];
}

Atom scheme_concat(List args) {
    Atom arg = args.data[0];

    if(arg.ty != ATOM_LIST) return arg;

    return GET_ATOM_LIST(arg).data[GET_ATOM_LIST(arg).length - 1];
}

Atom scheme_println(List args) {
    for(u64 i = 0; i < args.length; i++) {
        Printer_Atom(args.data[i]);
    }

    printf("\n");

    return ATOM_NIL();
}

Atom scheme_import(List args) {
    Atom arg = args.data[0];

    if(arg.ty != ATOM_STRING) return arg;

    Scheme_run_file(GET_ATOM_STRING(arg).data);

    return ATOM_NIL();
}

Atom scheme_eval(List args) {
    Atom arg = args.data[0];

    return Eval_Atom(*GET_ATOM_QUOTE(arg));
}

void Scheme_builtin(void) {
    INTRINSIC("+", scheme_add, -1);
    INTRINSIC("-", scheme_sub, -1);
    INTRINSIC("*", scheme_mul, -1);
    INTRINSIC("/", scheme_div, -1);

    INTRINSIC("=", scheme_eq, 2);
    INTRINSIC("!", scheme_not, 2);
    INTRINSIC(">", scheme_greater, 2);
    INTRINSIC("<", scheme_lesser, 2);

    INTRINSIC("and", scheme_and, 2);
    INTRINSIC("or", scheme_or, 2);

    GLOBAL("true", ATOM_BOOLEAN(true));
    GLOBAL("false", ATOM_BOOLEAN(false));

    GLOBAL("nil", ATOM_NIL());

    INTRINSIC("println", scheme_println, -1);

    INTRINSIC("import", scheme_import, 1);

    INTRINSIC("eval", scheme_eval, 1);

    INTRINSIC("head", scheme_head, 1);
    INTRINSIC("tail", scheme_tail, 1);
}

#define DEBUG_PRINT
#undef DEBUG_PRINT

void Scheme_run(const char* src) {
    Reader reader = Reader_make(String_make(src));

    Reader_run(&reader);

    Eval_Atom(reader.global);

    Reader_free(&reader);
}

void Scheme_run_file(const char* path) {
    FILE* file = fopen(path, "r");

    if(!file) {
        Scheme_error("Couldnt not find file");
        return;
    }

    fseek(file, 0, SEEK_END);
    u64 length = ftell(file);

    fseek(file, 0, SEEK_SET);
    char* buffer = MainAllocator_malloc(length);

    fread(buffer, 1, length, file);
    fclose(file);

    Scheme_run(buffer);
}
