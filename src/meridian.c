#include "meridian.h"

#include "meridian_allocator.h"
#include "meridian_atom.h"
#include "meridian_common.h"
#include "meridian_env.h"
#include "meridian_error.h"
#include "meridian_eval.h"
#include "meridian_printer.h"
#include "meridian_reader.h"

#include <llvm-c/Types.h>
#include <llvm-c/Core.h>

#include <stdio.h>
#include <stdlib.h>

void Meridian_init(void) {
    MainAllocator_init();
    Env_init();
}

void Meridian_free(void) {
    Env_free();
    MainAllocator_free();
}

#define INTRINSIC(name, fn, argc)\
    do {\
        Atom atom = ATOM_INTRINSIC(name, &fn, argc);\
        Env_set(String_make(name), atom);\
    } while(0);

#define GLOBAL(name, value) Env_set(String_make(name), value)

Atom meridian_add(List args) {
    Atom result = ATOM_INTEGER(0);

    for(u64 i = 0; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) += GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom meridian_sub(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) -= GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom meridian_mul(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) *= GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom meridian_div(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) /= GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom meridian_eq(List args) {
    Atom lhs = args.data[0];
    Atom rhs = args.data[1];

    if(lhs.ty != rhs.ty) {
        Meridian_error("Arguments must be the same type");
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

Atom meridian_not(List args) {
    Atom arg = args.data[0];

    if(arg.ty != ATOM_BOOLEAN) {
        Meridian_error("Arguments must be a boolean");
        return ATOM_NIL();
    }

    return ATOM_BOOLEAN(!GET_ATOM_BOOLEAN(arg));
}

Atom meridian_greater(List args) {
    Atom lhs = args.data[0];
    Atom rhs = args.data[1];

    if(lhs.ty != rhs.ty) {
        Meridian_error("Arguments must be the same type");
        return ATOM_NIL();
    }

    switch(lhs.ty) {
        case ATOM_INTEGER: return ATOM_BOOLEAN(GET_ATOM_INTEGER(lhs) > GET_ATOM_INTEGER(rhs));
        case ATOM_REAL: return ATOM_BOOLEAN(GET_ATOM_REAL(lhs) > GET_ATOM_REAL(rhs));
        default:
           Meridian_error("Invalid comparison types"); 
           return ATOM_NIL();
    }

    return ATOM_BOOLEAN(false);
}

Atom meridian_lesser(List args) {
    Atom lhs = args.data[0];
    Atom rhs = args.data[1];

    if(lhs.ty != rhs.ty) {
        Meridian_error("Arguments must be the same type");
        return ATOM_NIL();
    }

    switch(lhs.ty) {
        case ATOM_INTEGER: return ATOM_BOOLEAN(GET_ATOM_INTEGER(lhs) < GET_ATOM_INTEGER(rhs));
        case ATOM_REAL: return ATOM_BOOLEAN(GET_ATOM_REAL(lhs) < GET_ATOM_REAL(rhs));
        default:
           Meridian_error("Invalid comparison types"); 
           return ATOM_NIL();
    }

    return ATOM_BOOLEAN(false);
}

Atom meridian_and(List args) {
    for(u64 i = 0; i < args.length; i++) {
        if(!GET_ATOM_BOOLEAN(args.data[i])) {
            return ATOM_BOOLEAN(false);
        }
    }

    return ATOM_BOOLEAN(true);
}

Atom meridian_or(List args) {
    for(u64 i = 0; i < args.length; i++) {
        if(GET_ATOM_BOOLEAN(args.data[i])) {
            return ATOM_BOOLEAN(true);
        }
    }

    return ATOM_BOOLEAN(false);
}

Atom meridian_head(List args) {
    Atom arg = args.data[0];

    if(arg.ty != ATOM_LIST) return arg;

    return GET_ATOM_LIST(arg).data[0];
}

Atom meridian_tail(List args) {
    Atom arg = args.data[0];

    if(arg.ty != ATOM_LIST) return arg;

    return GET_ATOM_LIST(arg).data[GET_ATOM_LIST(arg).length - 1];
}

Atom meridian_concat(List args) {
    Atom arg = args.data[0];

    if(arg.ty != ATOM_LIST) return arg;

    return GET_ATOM_LIST(arg).data[GET_ATOM_LIST(arg).length - 1];
}

Atom meridian_println(List args) {
    for(u64 i = 0; i < args.length; i++) {
        Printer_Atom(args.data[i]);
    }

    printf("\n");

    return ATOM_NIL();
}

Atom meridian_import(List args) {
    Atom arg = args.data[0];

    if(arg.ty != ATOM_STRING) return arg;

    Meridian_run_file(GET_ATOM_STRING(arg).data);

    return ATOM_NIL();
}

Atom meridian_eval(List args) {
    Atom arg = args.data[0];

    return Eval_Atom(*GET_ATOM_QUOTE(arg));
}

void Meridian_builtin(void) {
    INTRINSIC("+", meridian_add, -1);
    INTRINSIC("-", meridian_sub, -1);
    INTRINSIC("*", meridian_mul, -1);
    INTRINSIC("/", meridian_div, -1);

    INTRINSIC("=", meridian_eq, 2);
    INTRINSIC("!", meridian_not, 2);
    INTRINSIC(">", meridian_greater, 2);
    INTRINSIC("<", meridian_lesser, 2);

    INTRINSIC("and", meridian_and, 2);
    INTRINSIC("or", meridian_or, 2);

    GLOBAL("true", ATOM_BOOLEAN(true));
    GLOBAL("false", ATOM_BOOLEAN(false));

    GLOBAL("nil", ATOM_NIL());

    INTRINSIC("println", meridian_println, -1);

    INTRINSIC("import", meridian_import, 1);

    INTRINSIC("eval", meridian_eval, 1);

    INTRINSIC("head", meridian_head, 1);
    INTRINSIC("tail", meridian_tail, 1);
}

#define DEBUG_PRINT
#undef DEBUG_PRINT

void Meridian_run(const char* src) {
    Reader reader = Reader_make(String_make(src));

    Reader_run(&reader);

    Eval_Atom(reader.global);

    Reader_free(&reader);
}

void Meridian_run_file(const char* path) {
    FILE* file = fopen(path, "r");

    if(!file) {
        Meridian_error("Couldnt not find file");
        return;
    }

    fseek(file, 0, SEEK_END);
    u64 length = ftell(file);

    fseek(file, 0, SEEK_SET);
    char* buffer = MainAllocator_malloc(length);

    fread(buffer, 1, length, file);
    fclose(file);

    Meridian_run(buffer);
}
