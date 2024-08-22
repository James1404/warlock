#include "warlock_eval.h"
#include "warlock_atom.h"
#include "warlock_builtins.h"
#include "warlock_error.h"
#include "warlock_string.h"

#include <stdlib.h>
#include <string.h>

void Eval_run(SexpAllocator* alloc, Sexp sexp) {
    Eval_Atom(alloc, sexp);
}

Sexp Eval_Atom(SexpAllocator* alloc, Sexp sexp) {
    switch(ATOM_TY(alloc, sexp)) {
    case ATOM_SYMBOL: {
        return Eval_Atom(alloc, SexpAllocator_getLocal(alloc, ATOM_VALUE(alloc, sexp, ATOM_SYMBOL)));
    } break;
    case ATOM_CONS: return Eval_Cons(alloc, sexp);
    case ATOM_QUOTE: return ATOM_VALUE(alloc, sexp, ATOM_QUOTE);

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

Sexp Eval_Quote(SexpAllocator* alloc, Sexp sexp) {
    if(ATOM_TY(alloc, sexp) != ATOM_CONS) return ATOM_MAKE_NIL(alloc);

    Cons cons = ATOM_VALUE(alloc, sexp, ATOM_CONS);

    if(!Eval_match(alloc, cons.data, "quote")) {
        return ATOM_MAKE_NIL(alloc);
    }

    if(ATOM_TY(alloc, cons.next) != ATOM_CONS) {
        return ATOM_MAKE_NIL(alloc);
    }

    cons = ATOM_VALUE(alloc, cons.next, ATOM_CONS);

    return ATOM_MAKE_V(alloc, ATOM_QUOTE, cons.data);
}

Sexp Eval_Cons(SexpAllocator* alloc, Sexp sexp) {
    if(ATOM_TY(alloc, sexp) != ATOM_CONS) return ATOM_MAKE_NIL(alloc);
    
    Sexp predicate = ATOM_VALUE(alloc, sexp, ATOM_CONS).data;

    if(ATOM_TY(alloc, predicate) == ATOM_SYMBOL) {
        if(STR_CMP_WITH_RAW(ATOM_VALUE(alloc, predicate, ATOM_SYMBOL), "quote")) {
            return Eval_Quote(alloc, sexp);
        }

        if(STR_CMP_WITH_RAW(ATOM_VALUE(alloc, predicate, ATOM_SYMBOL), "define")) {
            return Eval_Define(alloc, sexp);
        }

        if(STR_CMP_WITH_RAW(ATOM_VALUE(alloc, predicate, ATOM_SYMBOL), "if")) {
            return Eval_If(alloc, sexp);
        }

        if(STR_CMP_WITH_RAW(ATOM_VALUE(alloc, predicate, ATOM_SYMBOL), "lambda")) {
            return Eval_Lambda(alloc, sexp);
        }
    }

    Sexp current = sexp;

    while(!SexpAllocator_ConsTerminated(alloc, current)) {
        Sexp data = Sexp_First(alloc, current);
        ATOM_VALUE(alloc, current, ATOM_CONS).data = Eval_Atom(alloc, data);
        current = Sexp_Rest(alloc, current);
    }

    Sexp fn = Sexp_First(alloc, sexp);
    if(ATOM_TY(alloc, fn) == ATOM_INTRINSIC) {
        Sexp args = Sexp_Rest(alloc, sexp);
        Intrinsic intrinsic = ATOM_VALUE(alloc, fn, ATOM_INTRINSIC);

        i64 len = Sexp_Len(alloc, args);

        if((len == intrinsic.argc) || (intrinsic.argc == -1 && len > 0)) {
            return intrinsic.fn(alloc, args);
        }

        Warlock_error("invalid argument count");
        return ATOM_MAKE_NIL(alloc);
    }

    if(ATOM_TY(alloc, fn) == ATOM_FN) {
        Sexp args = Sexp_Rest(alloc, sexp);
        i64 len = Sexp_Len(alloc, args);

        SexpAllocator_incScope(alloc);

        Sexp result = ATOM_MAKE_NIL(alloc);

        if(len != ATOM_VALUE(alloc, fn, ATOM_FN).args) {
            Warlock_error("Invalid amount of arguments");
            return ATOM_MAKE_NIL(alloc);
        }

        Sexp currentPassedArg = args;
        Sexp currentFnArg = ATOM_VALUE(alloc, fn, ATOM_FN).args;

        while(!SexpAllocator_ConsTerminated(alloc, currentPassedArg) &&
              !SexpAllocator_ConsTerminated(alloc, currentFnArg)) {
            Sexp arg = Sexp_First(alloc, currentPassedArg);
            Sexp fnArg = Sexp_First(alloc, currentFnArg);
                        
            String name = ATOM_VALUE(alloc, fnArg, ATOM_SYMBOL);
            SexpAllocator_setLocal(alloc, name, arg);

            currentPassedArg = Sexp_Rest(alloc, currentPassedArg);
            currentFnArg = Sexp_Rest(alloc, currentFnArg);
        }

        result = Eval_Atom(alloc, ATOM_VALUE(alloc, fn, ATOM_FN) .body);

        SexpAllocator_decScope(alloc);

        return result;
    }


    return Sexp_First(alloc, sexp);
}

bool Eval_match(SexpAllocator* alloc, Sexp sexp, const char* expected) {
    if(ATOM_TY(alloc, sexp) == ATOM_SYMBOL) {
        String symbol = ATOM_VALUE(alloc, sexp, ATOM_SYMBOL);
        return STR_CMP_WITH_RAW(symbol, expected);
    }

    return false;
}

Sexp Eval_Define(SexpAllocator* alloc, Sexp sexp) {
    Sexp predicate = Sexp_First(alloc, sexp);

    if(Eval_match(alloc, predicate, "define")) {
        sexp = Sexp_Rest(alloc, sexp);

        Sexp symbol = Sexp_First(alloc, sexp);
        if(ATOM_TY(alloc, symbol) == ATOM_SYMBOL) {
            sexp = Sexp_Rest(alloc, sexp);

            Sexp value = Sexp_First(alloc, sexp);
            SexpAllocator_setLocal(alloc, ATOM_VALUE(alloc, symbol, ATOM_SYMBOL), value);
        }
    }

    return ATOM_MAKE_NIL(alloc);
}

Sexp Eval_If(SexpAllocator* alloc, Sexp sexp) {
    Sexp predicate = Sexp_First(alloc, sexp);
        
    if(!Eval_match(alloc, predicate, "if")) {
        return ATOM_MAKE_NIL(alloc);
    }

    sexp = Sexp_Rest(alloc, sexp);

    Sexp cond = Eval_Atom(alloc, Sexp_First(alloc, sexp));
    if(ATOM_TY(alloc, cond) != ATOM_BOOLEAN) {
        Warlock_error("condition must be a boolean");
        return ATOM_MAKE_NIL(alloc);
    }

    sexp = Sexp_Rest(alloc, sexp);
    Sexp t = Sexp_First(alloc, sexp);
    sexp = Sexp_Rest(alloc, sexp);
    Sexp f = Sexp_First(alloc, sexp);

    return Eval_Atom(alloc, ATOM_VALUE(alloc, cond, ATOM_BOOLEAN) ? t : f);
}

Sexp Eval_Lambda(SexpAllocator* alloc, Sexp sexp) {
    Sexp predicate = Sexp_First(alloc, sexp);
    if(!Eval_match(alloc, predicate, "lambda")) return ATOM_MAKE_NIL(alloc);

    sexp = Sexp_Rest(alloc, sexp);
    Sexp args = Sexp_First(alloc, sexp);
    sexp = Sexp_Rest(alloc, sexp);
    Sexp body = Sexp_First(alloc, sexp);

    Sexp current = args;
    while(!SexpAllocator_ConsTerminated(alloc, current)) {
        Sexp arg = Sexp_First(alloc, current);
        if(ATOM_TY(alloc, arg) != ATOM_SYMBOL) {
            Warlock_error("function parameters must all be symbols");
            return ATOM_MAKE_NIL(alloc);
        }
        current = Sexp_Rest(alloc, current);
    }

    return ATOM_MAKE_S(alloc, ATOM_FN, args, body);
}
