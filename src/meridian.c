#include "meridian.h"

#include "meridian_allocator.h"
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

#define INTRINSIC(name, fn, ret, varname, usevar)\
    do {\
        Atom atom = ATOM_INTRINSIC(name, &fn, ret);\
        Intrinsic* varname = &GET_ATOM_INTRINSIC(atom);\
        usevar\
        Env_set(String_make(name), atom);\
    } while(0);

#define INTRINSIC_VARARG(name, fn, ret, argtype) Env_set(String_make(name), ATOM_INTRINSIC_VARIADIC(name, &fn, ret, argtype))

#define GLOBAL(name, value) Env_set(String_make(name), value)

Atom meridian_add_int(List args) {
    Atom result = ATOM_INTEGER(0);

    for(u64 i = 0; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) += GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom meridian_sub_int(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) -= GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom meridian_mul_int(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) *= GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom meridian_div_int(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_INTEGER(result) /= GET_ATOM_INTEGER(args.data[i]);
    }

    return result;
}

Atom meridian_add_real(List args) {
    Atom result = ATOM_REAL(0);

    for(u64 i = 0; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_REAL(result) += GET_ATOM_REAL(args.data[i]);
    }

    return result;
}

Atom meridian_sub_real(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_REAL(result) -= GET_ATOM_REAL(args.data[i]);
    }

    return result;
}

Atom meridian_mul_real(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_REAL(result) *= GET_ATOM_REAL(args.data[i]);
    }

    return result;
}

Atom meridian_div_real(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_REAL(result) /= GET_ATOM_REAL(args.data[i]);
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

void Meridian_builtin(void) {
    INTRINSIC_VARARG("+", meridian_add_int, TYPE_INTEGER_ANY(), TYPE_INTEGER_ANY());
    INTRINSIC_VARARG("-", meridian_sub_int, TYPE_INTEGER_ANY(), TYPE_INTEGER_ANY());
    INTRINSIC_VARARG("*", meridian_mul_int, TYPE_INTEGER_ANY(), TYPE_INTEGER_ANY());
    INTRINSIC_VARARG("/", meridian_div_int, TYPE_INTEGER_ANY(), TYPE_INTEGER_ANY());

    INTRINSIC_VARARG("+.", meridian_add_real, TYPE_REAL_ANY(), TYPE_REAL_ANY());
    INTRINSIC_VARARG("-.", meridian_sub_real, TYPE_REAL_ANY(), TYPE_REAL_ANY());
    INTRINSIC_VARARG("*.", meridian_mul_real, TYPE_REAL_ANY(), TYPE_REAL_ANY());
    INTRINSIC_VARARG("/.", meridian_div_real, TYPE_REAL_ANY(), TYPE_REAL_ANY());

    INTRINSIC("=", meridian_eq, TYPE_BOOLEAN(), func, {
        Intrinsic_add_argument(func, TYPE_ANY());
        Intrinsic_add_argument(func, TYPE_ANY());
    });
    INTRINSIC("!", meridian_not, TYPE_BOOLEAN(), func, {
        Intrinsic_add_argument(func, TYPE_ANY());
    });

    INTRINSIC(">", meridian_greater, TYPE_BOOLEAN(), func, {
        Intrinsic_add_argument(func, TYPE_ANY());
        Intrinsic_add_argument(func, TYPE_ANY());
    });
    INTRINSIC("<", meridian_lesser, TYPE_BOOLEAN(), func, {
        Intrinsic_add_argument(func, TYPE_ANY());
        Intrinsic_add_argument(func, TYPE_ANY());
    });

    INTRINSIC("and", meridian_and, TYPE_BOOLEAN(), func, {
        Intrinsic_add_argument(func, TYPE_ANY());
        Intrinsic_add_argument(func, TYPE_ANY());
    });
    INTRINSIC("or", meridian_or, TYPE_BOOLEAN(), func, {
        Intrinsic_add_argument(func, TYPE_ANY());
        Intrinsic_add_argument(func, TYPE_ANY());
    });

    GLOBAL("true", ATOM_BOOLEAN(true));
    GLOBAL("false", ATOM_BOOLEAN(false));

    GLOBAL("nil", ATOM_NIL());

    INTRINSIC_VARARG("println", meridian_println, TYPE_ANY(), TYPE_UNIT());

    INTRINSIC("import", meridian_import, TYPE_UNIT(), func, {
        Intrinsic_add_argument(func, TYPE_STRING());
    });

    INTRINSIC("head", meridian_head, TYPE_ANY(), func, {
        Intrinsic_add_argument(func, TYPE_LIST());
    });
    INTRINSIC("tail", meridian_tail, TYPE_ANY(), func, {
        Intrinsic_add_argument(func, TYPE_LIST());
    });
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
