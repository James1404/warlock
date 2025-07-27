#include "warlock_eval.h"
#include "warlock_atom.h"
#include "warlock_builtins.h"
#include "warlock_error.h"
#include "warlock_string.h"

/* typedef struct { */
/*     const char* name; */
/*     Sexp (*fn)(Environment*, Sexp); */
/* } Builtin; */

/* static Builtin builtins[] = { */
/*     {"fn", &Eval_Fn}, */
/*     {"def", &Eval_Def}, */
/*     {"quote", &Eval_Quote}, */
/* }; */

Sexp Eval_run(Environment* env, Sexp sexp) { return Eval_Atom(env, sexp); }

Sexp Eval_Atom(Environment* env, Sexp sexp) {
    switch (ATOM_TY(sexp)) {
    case ATOM_SYMBOL: {
        return Environment_getLocal(env, ATOM_VALUE(sexp, ATOM_SYMBOL));
    } break;
    case ATOM_CONS:
        return Eval_Cons(env, sexp);
    case ATOM_LIST:
        return Eval_List(env, sexp);
    case ATOM_QUOTE:
        return ATOM_VALUE(sexp, ATOM_QUOTE);

    case ATOM_NUMBER:
    case ATOM_BOOLEAN:
    case ATOM_STRING:
    case ATOM_NIL:
    case ATOM_KEYWORD:
    case ATOM_FN:
    case ATOM_INTRINSIC:
    case ATOM_FFI:
        return sexp;
    }

    return ATOM_MAKE_NIL(env);
}

Sexp Eval_Cons(Environment* env, Sexp sexp) {
    if (ATOM_TY(sexp) != ATOM_CONS) {
        return Eval_Atom(env, sexp);
    }

    Sexp fn = Sexp_First(env, sexp);
    Sexp args = Sexp_Rest(env, sexp);

    fn = Eval_Atom(env, fn);

    switch (ATOM_TY(fn)) {
    case ATOM_INTRINSIC: {
        Intrinsic intrinsic = ATOM_VALUE(fn, ATOM_INTRINSIC);

        Sexp lenSexp = Sexp_Len(env, args);
        f64 len = ATOM_VALUE(lenSexp, ATOM_NUMBER);

        if (intrinsic.eval_all) {
            Sexp current = args;
            while (!Environment_ConsTerminated(env, current)) {
                Sexp data = Sexp_First(env, current);
                ATOM_VALUE(current, ATOM_CONS).data = Eval_Atom(env, data);
                current = Sexp_Rest(env, current);
            }
        }

        if (len == intrinsic.argc || (intrinsic.argc == -1 && len > 0)) {
            return intrinsic.fn(env, args);
        }

        Warlock_error("invalid argument count");
        return ATOM_MAKE_NIL(env);
    } break;
    case ATOM_FN: {
        Sexp args = Sexp_Rest(env, sexp);
        u64 len = Environment_ConsLen(env, args);

        Environment_incScope(env);

        Sexp result = ATOM_MAKE_NIL(env);

        if (len != Environment_ConsLen(env, ATOM_VALUE(fn, ATOM_FN).args)) {
            Warlock_error("Invalid amount of arguments");
            return ATOM_MAKE_NIL(env);
        }

        Sexp currentPassedArg = args;
        Sexp currentFnArg = ATOM_VALUE(fn, ATOM_FN).args;

        while (!Environment_ConsTerminated(env, currentPassedArg) &&
               !Environment_ConsTerminated(env, currentFnArg)) {
            Sexp arg = Sexp_First(env, currentPassedArg);
            Sexp fnArg = Sexp_First(env, currentFnArg);

            String name = ATOM_VALUE(fnArg, ATOM_SYMBOL);
            Environment_setLocal(env, name, arg);

            currentPassedArg = Sexp_Rest(env, currentPassedArg);
            currentFnArg = Sexp_Rest(env, currentFnArg);
        }

        result = Eval_Atom(env, ATOM_VALUE(fn, ATOM_FN).body);

        Environment_decScope(env);

        return result;
    } break;
    default:
        break;
    }

    return fn;
}

Sexp Eval_List(Environment* env, Sexp sexp) {
    return ATOM_MAKE_NIL(env); // TODO: Finish
}

Sexp Eval_Fn(Environment* env, Sexp sexp) {
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

Sexp Eval_Def(Environment* env, Sexp sexp) {
    Sexp symbol = Sexp_First(env, sexp);
    if (ATOM_TY(symbol) == ATOM_SYMBOL) {
        sexp = Sexp_Rest(env, sexp);

        Sexp value = Sexp_First(env, sexp);
        Environment_setLocal(env, ATOM_VALUE(symbol, ATOM_SYMBOL), value);
    }

    return ATOM_MAKE_NIL(env);
}

Sexp Eval_Quote(Environment* env, Sexp sexp) {
    return ATOM_MAKE_V(env, ATOM_QUOTE, sexp);
}
