#include "warlock_builtins.h"
#include "warlock.h"
#include "warlock_atom.h"
#include "warlock_error.h"
#include "warlock_eval.h"
#include "warlock_string.h"

#include <stdio.h>

#define EXPECTED_TYPE(sexp, expected)                                          \
    if (ATOM_TY(sexp) != expected) {                                           \
        Warlock_error("Expected '%s'", #expected);                             \
        return ATOM_MAKE(env, ATOM_NIL);                                       \
    }

Sexp Sexp_Def(Environment* env, Sexp sexp) {
    Sexp symbol = Sexp_First(env, sexp);

    if (ATOM_TY(symbol) == ATOM_SYMBOL) {
        String name = String_copy(ATOM_VALUE(symbol, ATOM_SYMBOL));

        sexp = Sexp_Rest(env, sexp);

        Sexp value = Eval_Atom(env, Sexp_First(env, sexp));

        Environment_setLocal(env, name, value);
    }

    return ATOM_MAKE_NIL(env);
}

Sexp Sexp_Fn(Environment* env, Sexp sexp) {
    Sexp args = Sexp_First(env, sexp);
    sexp = Sexp_Rest(env, sexp);
    Sexp body = Sexp_First(env, sexp);

    Sexp current = args;

    while (!Environment_ConsTerminated(env, current)) {
        Sexp arg = Sexp_First(env, current);
        if (ATOM_TY(arg) != ATOM_SYMBOL) {
            Warlock_error("function parameters must all be symbols");
            return ATOM_MAKE_NIL(env);
        }
        current = Sexp_Rest(env, current);
    }

    return ATOM_MAKE_S(env, ATOM_FN, args, body);
}

Sexp Sexp_Quote(Environment* env, Sexp sexp) {
    return ATOM_MAKE_V(env, ATOM_QUOTE, sexp);
}

Sexp Sexp_PrintEnv(Environment* env, Sexp sexp) {
    printf("--- Environment :: Locals %lu ---\n", env->localsLen);
    for (i32 i = env->localsLen - 1; i >= 0; i--) {
        Local local = env->locals[i];

        printf("%.*s :: ", local.name.len, local.name.raw);
        Environment_print(env, local.sexp);
        printf("\n");
    }

    return ATOM_MAKE_NIL(env);
}

Sexp Sexp_Add(Environment* env, Sexp sexp) {
    Sexp result = ATOM_MAKE_V(env, ATOM_NUMBER, 0.0f);

    Sexp current = sexp;

    while (!Environment_ConsTerminated(env, current)) {
        Sexp data = Sexp_First(env, current);
        ATOM_VALUE(result, ATOM_NUMBER) += ATOM_VALUE(data, ATOM_NUMBER);

        current = Sexp_Rest(env, current);
    }

    return result;
}

Sexp Sexp_Subtract(Environment* env, Sexp sexp) {
    Sexp result = ATOM_MAKE_V(env, ATOM_NUMBER, 0.0f);

    Sexp current = sexp;
    ATOM_VALUE(result, ATOM_NUMBER) =
        ATOM_VALUE(Sexp_First(env, current), ATOM_NUMBER);

    current = Sexp_Rest(env, current);

    while (!Environment_ConsTerminated(env, current)) {
        Sexp data = ATOM_VALUE(current, ATOM_CONS).data;
        ATOM_VALUE(result, ATOM_NUMBER) -= ATOM_VALUE(data, ATOM_NUMBER);

        current = ATOM_VALUE(current, ATOM_CONS).next;
    }

    return result;
}

Sexp Sexp_Multiply(Environment* env, Sexp sexp) {
    Sexp result = ATOM_MAKE_V(env, ATOM_NUMBER, 0.0f);

    Sexp current = sexp;
    ATOM_VALUE(result, ATOM_NUMBER) =
        ATOM_VALUE(Sexp_First(env, current), ATOM_NUMBER);

    current = Sexp_Rest(env, current);

    while (!Environment_ConsTerminated(env, current)) {
        Sexp data = ATOM_VALUE(current, ATOM_CONS).data;
        ATOM_VALUE(result, ATOM_NUMBER) *= ATOM_VALUE(data, ATOM_NUMBER);

        current = ATOM_VALUE(current, ATOM_CONS).next;
    }

    return result;
}

Sexp Sexp_Divide(Environment* env, Sexp sexp) {
    Sexp result = ATOM_MAKE_V(env, ATOM_NUMBER, 0.0f);

    Sexp current = sexp;
    ATOM_VALUE(result, ATOM_NUMBER) =
        ATOM_VALUE(Sexp_First(env, current), ATOM_NUMBER);

    current = Sexp_Rest(env, current);

    while (!Environment_ConsTerminated(env, current)) {
        Sexp data = ATOM_VALUE(current, ATOM_CONS).data;
        ATOM_VALUE(result, ATOM_NUMBER) /= ATOM_VALUE(data, ATOM_NUMBER);

        current = ATOM_VALUE(current, ATOM_CONS).next;
    }

    return result;
}

Sexp Sexp_Equal(Environment* env, Sexp sexp) {
    Sexp lhs = Sexp_First(env, sexp);
    sexp = Sexp_Rest(env, sexp);
    Sexp rhs = Sexp_First(env, sexp);

    if (ATOM_TY(lhs) != ATOM_TY(rhs)) {
        Warlock_error("Arguments must be the same type");
        return ATOM_MAKE_V(env, ATOM_BOOLEAN, false);
    }

    bool result = false;
    switch (ATOM_TY(lhs)) {
    case ATOM_NUMBER:
        result = ATOM_VALUE(lhs, ATOM_NUMBER) == ATOM_VALUE(rhs, ATOM_NUMBER);
        break;
    case ATOM_BOOLEAN:
        result = ATOM_VALUE(lhs, ATOM_BOOLEAN) == ATOM_VALUE(rhs, ATOM_BOOLEAN);
        break;
    case ATOM_STRING:
        result =
            String_cmp(ATOM_VALUE(lhs, ATOM_STRING), ATOM_VALUE(rhs, ATOM_STRING));
        break;
    case ATOM_SYMBOL:
        result =
            String_cmp(ATOM_VALUE(lhs, ATOM_SYMBOL), ATOM_VALUE(rhs, ATOM_SYMBOL));
        break;
    case ATOM_KEYWORD:
        result = String_cmp(ATOM_VALUE(lhs, ATOM_KEYWORD),
                         ATOM_VALUE(rhs, ATOM_KEYWORD));
        break;
    default:
        break;
    }

    return ATOM_MAKE_V(env, ATOM_BOOLEAN, result);
}

Sexp Sexp_Not(Environment* env, Sexp sexp) {
    Sexp arg = Sexp_First(env, sexp);

    EXPECTED_TYPE(arg, ATOM_BOOLEAN);

    bool v = ATOM_VALUE(arg, ATOM_BOOLEAN);
    return ATOM_MAKE_V(env, ATOM_BOOLEAN, !v);
}

Sexp Sexp_Greater(Environment* env, Sexp sexp) {
    Sexp lhs = Sexp_First(env, sexp);
    sexp = Sexp_Rest(env, sexp);
    Sexp rhs = Sexp_First(env, sexp);

    if (ATOM_TY(lhs) != ATOM_TY(rhs)) {
        Warlock_error("Arguments must be the same type");
        return ATOM_MAKE_V(env, ATOM_BOOLEAN, false);
    }

    bool result = false;

    switch (ATOM_TY(lhs)) {
    case ATOM_NUMBER:
        result = ATOM_VALUE(lhs, ATOM_NUMBER) > ATOM_VALUE(rhs, ATOM_NUMBER);
        break;
    default:
        Warlock_error("Invalid comparison types");
    }

    return ATOM_MAKE_V(env, ATOM_BOOLEAN, result);
}

Sexp Sexp_Less(Environment* env, Sexp sexp) {
    Sexp lhs = Sexp_First(env, sexp);
    sexp = Sexp_Rest(env, sexp);
    Sexp rhs = Sexp_First(env, sexp);

    if (ATOM_TY(lhs) != ATOM_TY(rhs)) {
        Warlock_error("Arguments must be the same type");
        return ATOM_MAKE_V(env, ATOM_BOOLEAN, false);
    }

    bool result = false;

    switch (ATOM_TY(lhs)) {
    case ATOM_NUMBER:
        result = ATOM_VALUE(lhs, ATOM_NUMBER) < ATOM_VALUE(rhs, ATOM_NUMBER);
        break;
    default:
        Warlock_error("Invalid comparison types");
    }

    return ATOM_MAKE_V(env, ATOM_BOOLEAN, result);
}

Sexp Sexp_And(Environment* env, Sexp sexp) {
    bool result = true;

    Sexp current = sexp;

    while (!Environment_ConsTerminated(env, current)) {
        Sexp data = Sexp_First(env, current);

        result = result && ATOM_VALUE(data, ATOM_BOOLEAN);

        current = Sexp_Rest(env, current);
    }

    return ATOM_MAKE_V(env, ATOM_BOOLEAN, result);
}

Sexp Sexp_Or(Environment* env, Sexp sexp) {
    bool result = true;

    Sexp current = sexp;

    while (!Environment_ConsTerminated(env, current)) {
        Sexp data = Sexp_First(env, current);

        result = result && ATOM_VALUE(data, ATOM_BOOLEAN);

        current = Sexp_Rest(env, current);
    }

    return ATOM_MAKE_V(env, ATOM_BOOLEAN, result);
}

static void Println_print(Environment* env, Sexp sexp) {
    switch (ATOM_TY(sexp)) {
    case ATOM_NUMBER: {
        printf("%Lf", ATOM_VALUE(sexp, ATOM_NUMBER));
    } break;
    case ATOM_BOOLEAN: {
        bool v = ATOM_VALUE(sexp, ATOM_NUMBER);
        printf("%s", v ? "#t" : "#f");
    } break;
    case ATOM_STRING: {
        String v = ATOM_VALUE(sexp, ATOM_STRING);
        printf("%.*s", v.len, v.raw);
    } break;
    case ATOM_SYMBOL: {
        String v = ATOM_VALUE(sexp, ATOM_SYMBOL);
        printf("%.*s", v.len, v.raw);
    } break;
    case ATOM_KEYWORD: {
        String v = ATOM_VALUE(sexp, ATOM_KEYWORD);
        printf(":%.*s", v.len, v.raw);
    } break;
    case ATOM_FN: {
        Fn fn = ATOM_VALUE(sexp, ATOM_FN);

        printf("(");

        printf("fn ");

        Println_print(env, fn.args);

        printf(" -> ");

        Println_print(env, fn.body);

        printf(")");
    } break;
    case ATOM_INTRINSIC: {
        Intrinsic intrinsic = ATOM_VALUE(sexp, ATOM_INTRINSIC);
        printf("#intrinsic %s, argc: %li", intrinsic.name, intrinsic.argc);
    } break;
    case ATOM_FFI: {
        printf("#ffi");
    } break;
    case ATOM_CONS: {
        printf("(");

        Sexp current = sexp;

        while (ATOM_TY(current) != ATOM_NIL) {
            Sexp head = Sexp_First(env, current);

            Println_print(env, head);

            if (ATOM_TY(current) == ATOM_CONS &&
                ATOM_VALUE(current, ATOM_CONS).next->ty != ATOM_NIL) {
                printf(" ");
            }

            current = Sexp_Rest(env, current);
        }

        printf(")");
    } break;
    case ATOM_LIST: {
        List list = ATOM_VALUE(sexp, ATOM_LIST);

        printf("(");

        for (i32 i = 0; i < list.len; i++) {
            Sexp value = list.data[i];
            Println_print(env, value);
        }

        printf(")");
    } break;

    case ATOM_QUOTE: {
        printf("'");
        Println_print(env, ATOM_VALUE(sexp, ATOM_QUOTE));
    } break;
    case ATOM_NIL: {
        printf("nil");
    } break;
    }
}

Sexp Sexp_Println(Environment* env, Sexp sexp) {
    Sexp current = sexp;

    while (!Environment_ConsTerminated(env, current)) {
        Sexp data = Sexp_First(env, current);

        Println_print(env, data);
        printf(" ");

        current = Sexp_Rest(env, current);
    }

    printf("\n");

    return ATOM_MAKE_NIL(env);
}

Sexp Sexp_First(Environment* env, Sexp sexp) {
    if (ATOM_TY(sexp) == ATOM_CONS) {
        return ATOM_VALUE(sexp, ATOM_CONS).data;
    }

    return sexp;
}

Sexp Sexp_Rest(Environment* env, Sexp sexp) {
    if (ATOM_TY(sexp) == ATOM_CONS) {
        return ATOM_VALUE(sexp, ATOM_CONS).next;
    }

    return ATOM_MAKE_NIL(env);
}

Sexp Sexp_Len(Environment* env, Sexp sexp) {
    Sexp result = ATOM_MAKE_V(env, ATOM_NUMBER, 0.0f);

    Sexp current = sexp;

    while (!Environment_ConsTerminated(env, current)) {
        ATOM_VALUE(result, ATOM_NUMBER) += 1.0f;
        current = ATOM_VALUE(current, ATOM_CONS).next;
    }

    return result;
}

Sexp Sexp_Concat(Environment* env, Sexp sexp) {
    return ATOM_MAKE(env, ATOM_NIL);
}

Sexp Sexp_Eval(Environment* env, Sexp sexp) {
    Sexp arg = Sexp_First(env, sexp);

    return Eval_Atom(env, ATOM_VALUE(arg, ATOM_QUOTE));
}

Sexp Sexp_Import(Environment* env, Sexp sexp) {
    Sexp arg = Sexp_First(env, sexp);

    if (ATOM_TY(arg) == ATOM_STRING) {
        Warlock_run_file(env, ATOM_VALUE(arg, ATOM_STRING));
    }

    return ATOM_MAKE(env, ATOM_NIL);
}

Sexp Sexp_If(Environment* env, Sexp sexp) {
    Sexp cond = Eval_Atom(env, Sexp_First(env, sexp));
    if (ATOM_TY(cond) != ATOM_BOOLEAN) {
        Warlock_error("condition must be a boolean");
        return ATOM_MAKE_NIL(env);
    }

    sexp = Sexp_Rest(env, sexp);
    Sexp t = Sexp_First(env, sexp);
    sexp = Sexp_Rest(env, sexp);
    Sexp f = Sexp_First(env, sexp);

    return Eval_Atom(env, ATOM_VALUE(cond, ATOM_BOOLEAN) ? t : f);
}
