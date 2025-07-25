#include "warlock_eval.h"
#include "warlock_atom.h"
#include "warlock_builtins.h"
#include "warlock_error.h"
#include "warlock_string.h"

typedef struct {
    const char* name;
    Sexp (*fn)(Environment *, Sexp);
} Builtin;

static Builtin builtins[] = {
    {"fn", &Eval_Fn},
    {"def", &Eval_Def},
    {"quote", &Eval_Quote},
};

Sexp Eval_run(Environment* alloc, Sexp sexp) {
    return Eval_Atom(alloc, sexp);
}

Sexp Eval_Atom(Environment* alloc, Sexp sexp) {
    switch(ATOM_TY(sexp)) {
    case ATOM_SYMBOL: {
        return Environment_getLocal(alloc, ATOM_VALUE(sexp, ATOM_SYMBOL));
    } break;
    case ATOM_CONS: return Eval_Cons(alloc, sexp);
    case ATOM_LIST: return Eval_List(alloc, sexp);
    case ATOM_QUOTE: return ATOM_VALUE(sexp, ATOM_QUOTE);

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

    return ATOM_MAKE_NIL(alloc);
}
  
Sexp Eval_Cons(Environment* alloc, Sexp sexp) {
    if(ATOM_TY(sexp) != ATOM_CONS) {
        return Eval_Atom(alloc, sexp);
    }

    Sexp fn = Sexp_First(alloc, sexp);
    Sexp args = Sexp_Rest(alloc, sexp);

    if(ATOM_TY(fn) == ATOM_SYMBOL) {
        String symbol = ATOM_VALUE(fn, ATOM_SYMBOL);

        for(u32 i = 0; i < ARRAY_LEN(builtins); i++) {
            if(STR_CMP_WITH_RAW(symbol, builtins[i].name)) {
                return builtins[i].fn(alloc, args);
            }
        }
    }
    
    Sexp current = sexp;
    while(!Environment_ConsTerminated(alloc, current)) {
        Sexp data = Sexp_First(alloc, current);
        ATOM_VALUE(current, ATOM_CONS).data = Eval_Atom(alloc, data);
        current = Sexp_Rest(alloc, current);
    }

    fn = Sexp_First(alloc, sexp);

    if(ATOM_TY(fn) == ATOM_INTRINSIC) {
        Intrinsic intrinsic = ATOM_VALUE(fn, ATOM_INTRINSIC);

        Sexp lenSexp = Sexp_Len(alloc, args);
        f64 len = ATOM_VALUE(lenSexp, ATOM_NUMBER);

        if(len == intrinsic.argc || (intrinsic.argc == -1 && len > 0)) {
            return intrinsic.fn(alloc, args);
        }

        Warlock_error("invalid argument count");
        return ATOM_MAKE_NIL(alloc);
    }

    if(ATOM_TY(fn) == ATOM_FN) {
        Sexp args = Sexp_Rest(alloc, sexp);
        u64 len = Environment_ConsLen(alloc, args);

        Environment_incScope(alloc);

        Sexp result = ATOM_MAKE_NIL(alloc);

        if(len != Environment_ConsLen(alloc, ATOM_VALUE(fn, ATOM_FN).args)) {
            Warlock_error("Invalid amount of arguments");
            return ATOM_MAKE_NIL(alloc);
        }

        Sexp currentPassedArg = args;
        Sexp currentFnArg = ATOM_VALUE(fn, ATOM_FN).args;

        while(!Environment_ConsTerminated(alloc, currentPassedArg) &&
              !Environment_ConsTerminated(alloc, currentFnArg)) {
            Sexp arg = Sexp_First(alloc, currentPassedArg);
            Sexp fnArg = Sexp_First(alloc, currentFnArg);
                        
            String name = ATOM_VALUE(fnArg, ATOM_SYMBOL);
            Environment_setLocal(alloc, name, arg);

            currentPassedArg = Sexp_Rest(alloc, currentPassedArg);
            currentFnArg = Sexp_Rest(alloc, currentFnArg);
        }

        result = Eval_Atom(alloc, ATOM_VALUE(fn, ATOM_FN) .body);

        Environment_decScope(alloc);

        return result;
    }

    return sexp;
}

Sexp Eval_List(Environment* alloc, Sexp sexp) {
    if(ATOM_TY(sexp) != ATOM_CONS) {
        return Eval_Atom(alloc, sexp);
    }

    Sexp fn = Sexp_First(alloc, sexp);
    Sexp args = Sexp_Rest(alloc, sexp);

    if(ATOM_TY(fn) == ATOM_SYMBOL) {
        String symbol = ATOM_VALUE(fn, ATOM_SYMBOL);

        for(u32 i = 0; i < ARRAY_LEN(builtins); i++) {
            if(STR_CMP_WITH_RAW(symbol, builtins[i].name)) {
                return builtins[i].fn(alloc, args);
            }
        }
    }
    
    Sexp current = sexp;
    while(!Environment_ConsTerminated(alloc, current)) {
        Sexp data = Sexp_First(alloc, current);
        ATOM_VALUE(current, ATOM_CONS).data = Eval_Atom(alloc, data);
        current = Sexp_Rest(alloc, current);
    }

    fn = Sexp_First(alloc, sexp);

    if(ATOM_TY(fn) == ATOM_INTRINSIC) {
        Intrinsic intrinsic = ATOM_VALUE(fn, ATOM_INTRINSIC);

        Sexp lenSexp = Sexp_Len(alloc, args);
        f64 len = ATOM_VALUE(lenSexp, ATOM_NUMBER);

        if(len == intrinsic.argc || (intrinsic.argc == -1 && len > 0)) {
            return intrinsic.fn(alloc, args);
        }

        Warlock_error("invalid argument count");
        return ATOM_MAKE_NIL(alloc);
    }

    if(ATOM_TY(fn) == ATOM_FN) {
        Sexp args = Sexp_Rest(alloc, sexp);
        u64 len = Environment_ConsLen(alloc, args);

        Environment_incScope(alloc);

        Sexp result = ATOM_MAKE_NIL(alloc);

        if(len != Environment_ConsLen(alloc, ATOM_VALUE(fn, ATOM_FN).args)) {
            Warlock_error("Invalid amount of arguments");
            return ATOM_MAKE_NIL(alloc);
        }

        Sexp currentPassedArg = args;
        Sexp currentFnArg = ATOM_VALUE(fn, ATOM_FN).args;

        while(!Environment_ConsTerminated(alloc, currentPassedArg) &&
              !Environment_ConsTerminated(alloc, currentFnArg)) {
            Sexp arg = Sexp_First(alloc, currentPassedArg);
            Sexp fnArg = Sexp_First(alloc, currentFnArg);
                        
            String name = ATOM_VALUE(fnArg, ATOM_SYMBOL);
            Environment_setLocal(alloc, name, arg);

            currentPassedArg = Sexp_Rest(alloc, currentPassedArg);
            currentFnArg = Sexp_Rest(alloc, currentFnArg);
        }

        result = Eval_Atom(alloc, ATOM_VALUE(fn, ATOM_FN) .body);

        Environment_decScope(alloc);

        return result;
    }

    return sexp;
}

Sexp Eval_Fn(Environment *alloc, Sexp sexp) {
    Sexp args = Sexp_First(alloc, sexp);
    sexp = Sexp_Rest(alloc, sexp);
    Sexp body = Sexp_First(alloc, sexp);

    Sexp current = args;
    
    while(!Environment_ConsTerminated(alloc, current)) {
        Sexp arg = Sexp_First(alloc, current);
        if(ATOM_TY(arg) != ATOM_SYMBOL) {
            Warlock_error("function parameters must all be symbols");
            return ATOM_MAKE_NIL(alloc);
        }
        current = Sexp_Rest(alloc, current);
    }

    return ATOM_MAKE_S(alloc, ATOM_FN, args, body);  
}

Sexp Eval_Def(Environment *alloc, Sexp sexp) {
    Sexp symbol = Sexp_First(alloc, sexp);
    if(ATOM_TY(symbol) == ATOM_SYMBOL) {
        sexp = Sexp_Rest(alloc, sexp);

        Sexp value = Sexp_First(alloc, sexp);
        Environment_setLocal(alloc, ATOM_VALUE(symbol, ATOM_SYMBOL), value);
    }

    return ATOM_MAKE_NIL(alloc);  
}

Sexp Eval_Quote(Environment *alloc, Sexp sexp) {
    return ATOM_MAKE_V(alloc, ATOM_QUOTE, sexp);  
}
