#include "warlock_builtins.h"
#include "warlock.h"
#include "warlock_atom.h"
#include "warlock_error.h"
#include "warlock_eval.h"

#include <stdio.h>

#define EXPECTED_TYPE(sexp, expected)                                   \
    if (ATOM_TY(sexp) != expected) {                             \
    Warlock_error("Expected '%s'", #expected);                          \
    return ATOM_MAKE(alloc, ATOM_NIL);                                  \
    }


Sexp Sexp_Add(Environment *alloc, Sexp sexp) {
    Sexp result = ATOM_MAKE_V(alloc, ATOM_NUMBER, 0.0f);

    Sexp current = sexp;

    while(!Environment_ConsTerminated(alloc, current)) {
        Sexp data = Sexp_First(alloc, current);
        ATOM_VALUE(result, ATOM_NUMBER) += ATOM_VALUE(data, ATOM_NUMBER);

        current = Sexp_Rest(alloc, current);
    }

    return result;
}

Sexp Sexp_Subtract(Environment *alloc, Sexp sexp) {
    Sexp result = ATOM_MAKE_V(alloc, ATOM_NUMBER, 0.0f);

    Sexp current = sexp;
    ATOM_VALUE(result, ATOM_NUMBER) = ATOM_VALUE(Sexp_First(alloc, current), ATOM_NUMBER);

    current = Sexp_Rest(alloc, current);

    while(!Environment_ConsTerminated(alloc, current)) {
        Sexp data = ATOM_VALUE(current, ATOM_CONS).data;
        ATOM_VALUE(result, ATOM_NUMBER) -= ATOM_VALUE(data, ATOM_NUMBER);
        
        current = ATOM_VALUE(current, ATOM_CONS).next;
    }

    return result;
}

Sexp Sexp_Multiply(Environment *alloc, Sexp sexp) {
    Sexp result = ATOM_MAKE_V(alloc, ATOM_NUMBER, 0.0f);

    Sexp current = sexp;
    ATOM_VALUE(result, ATOM_NUMBER) = ATOM_VALUE(Sexp_First(alloc, current), ATOM_NUMBER);

    current = Sexp_Rest(alloc, current);

    while(!Environment_ConsTerminated(alloc, current)) {
        Sexp data = ATOM_VALUE(current, ATOM_CONS).data;
        ATOM_VALUE(result, ATOM_NUMBER) *= ATOM_VALUE(data, ATOM_NUMBER);
        
        current = ATOM_VALUE(current, ATOM_CONS).next;
    }

    return result;
}

Sexp Sexp_Divide(Environment *alloc, Sexp sexp) {
    Sexp result = ATOM_MAKE_V(alloc, ATOM_NUMBER, 0.0f);

    Sexp current = sexp;
    ATOM_VALUE(result, ATOM_NUMBER) = ATOM_VALUE(Sexp_First(alloc, current), ATOM_NUMBER);

    current = Sexp_Rest(alloc, current);

    while(!Environment_ConsTerminated(alloc, current)) {
        Sexp data = ATOM_VALUE(current, ATOM_CONS).data;
        ATOM_VALUE(result, ATOM_NUMBER) /= ATOM_VALUE(data, ATOM_NUMBER);
        
        current = ATOM_VALUE(current, ATOM_CONS).next;
    }

    return result;  
}

Sexp Sexp_Equal(Environment *alloc, Sexp sexp) {
    Sexp lhs = Sexp_First(alloc, sexp);
    sexp = Sexp_Rest(alloc, sexp);
    Sexp rhs = Sexp_First(alloc, sexp);

    if(ATOM_TY(lhs) != ATOM_TY(rhs)) {
        Warlock_error("Arguments must be the same type");
        return ATOM_MAKE_V(alloc, ATOM_BOOLEAN, false);
    }

    bool result = false;
    switch(ATOM_TY(lhs)) {
    case ATOM_NUMBER:
        result = ATOM_VALUE(lhs, ATOM_NUMBER) == ATOM_VALUE(rhs, ATOM_NUMBER);
        break;
    case ATOM_BOOLEAN:
        result = ATOM_VALUE(lhs, ATOM_BOOLEAN) == ATOM_VALUE(rhs, ATOM_BOOLEAN);
        break;
    case ATOM_STRING:
        result = STR_CMP(ATOM_VALUE(lhs, ATOM_STRING), ATOM_VALUE(rhs, ATOM_STRING));
        break;
    case ATOM_SYMBOL:
        result = STR_CMP(ATOM_VALUE(lhs, ATOM_SYMBOL), ATOM_VALUE(rhs, ATOM_SYMBOL));
        break;
    case ATOM_KEYWORD:
        result = STR_CMP(ATOM_VALUE(lhs, ATOM_KEYWORD), ATOM_VALUE(rhs, ATOM_KEYWORD));
        break;
    default:
        break;
    }

    return ATOM_MAKE_V(alloc, ATOM_BOOLEAN, result);
}

Sexp Sexp_Not(Environment *alloc, Sexp sexp) {
    Sexp arg = Sexp_First(alloc, sexp);

    EXPECTED_TYPE(arg, ATOM_BOOLEAN);

    bool v = ATOM_VALUE(arg, ATOM_BOOLEAN);
    return ATOM_MAKE_V(alloc, ATOM_BOOLEAN, !v);  
}

Sexp Sexp_Greater(Environment *alloc, Sexp sexp) {
    Sexp lhs = Sexp_First(alloc, sexp);
    sexp = Sexp_Rest(alloc, sexp);
    Sexp rhs = Sexp_First(alloc, sexp);

    if(ATOM_TY(lhs) != ATOM_TY(rhs)) {
        Warlock_error("Arguments must be the same type");
        return ATOM_MAKE_V(alloc, ATOM_BOOLEAN, false);
    }

    bool result = false;

    switch(ATOM_TY(lhs)) {
    case ATOM_NUMBER:
        result = ATOM_VALUE(lhs, ATOM_NUMBER) > ATOM_VALUE(rhs, ATOM_NUMBER);
        break;
    default:
        Warlock_error("Invalid comparison types"); 
    }
    
    return ATOM_MAKE_V(alloc, ATOM_BOOLEAN, result);
}

Sexp Sexp_Less(Environment *alloc, Sexp sexp) {
    Sexp lhs = Sexp_First(alloc, sexp);
    sexp = Sexp_Rest(alloc, sexp);
    Sexp rhs = Sexp_First(alloc, sexp);

    if(ATOM_TY(lhs) != ATOM_TY(rhs)) {
        Warlock_error("Arguments must be the same type");
        return ATOM_MAKE_V(alloc, ATOM_BOOLEAN, false);
    }

    bool result = false;

    switch(ATOM_TY(lhs)) {
    case ATOM_NUMBER:
        result = ATOM_VALUE(lhs, ATOM_NUMBER) < ATOM_VALUE(rhs, ATOM_NUMBER);
        break;
    default:
        Warlock_error("Invalid comparison types"); 
    }
    
    return ATOM_MAKE_V(alloc, ATOM_BOOLEAN, result);
}

Sexp Sexp_And(Environment *alloc, Sexp sexp) {
    bool result = true;

    Sexp current = sexp;

    while(!Environment_ConsTerminated(alloc, current)) {
        Sexp data = Sexp_First(alloc, current);

        result = result && ATOM_VALUE(data, ATOM_BOOLEAN);

        current = Sexp_Rest(alloc, current);
    }

    return ATOM_MAKE_V(alloc, ATOM_BOOLEAN, result);
}

Sexp Sexp_Or(Environment *alloc, Sexp sexp) {
    bool result = true;

    Sexp current = sexp;

    while(!Environment_ConsTerminated(alloc, current)) {
        Sexp data = Sexp_First(alloc, current);

        result = result && ATOM_VALUE(data, ATOM_BOOLEAN);

        current = Sexp_Rest(alloc, current);
    }

    return ATOM_MAKE_V(alloc, ATOM_BOOLEAN, result);
}

Sexp Sexp_Println(Environment *alloc, Sexp sexp) {
    Sexp current = sexp;

    while(!Environment_ConsTerminated(alloc, current)) {
        Sexp data = Sexp_First(alloc, current);

        Environment_print(alloc, data);
        printf(" ");

        current = Sexp_Rest(alloc, current);
    }
    
    printf("\n");

    return ATOM_MAKE_NIL(alloc);
}

Sexp Sexp_First(Environment *alloc, Sexp sexp) {
    if(ATOM_TY(sexp) == ATOM_CONS) {
        return ATOM_VALUE(sexp, ATOM_CONS).data;
    }
    
    return sexp;
}

Sexp Sexp_Rest(Environment *alloc, Sexp sexp) {
    if(ATOM_TY(sexp) == ATOM_CONS) {
        return ATOM_VALUE(sexp, ATOM_CONS).next;
    }
    
    return ATOM_MAKE_NIL(alloc);
}

Sexp Sexp_Len(Environment* alloc, Sexp sexp) {
    Sexp result = ATOM_MAKE_V(alloc, ATOM_NUMBER, 0.0f);

    Sexp current = sexp;

    while(!Environment_ConsTerminated(alloc, current)) {
        ATOM_VALUE(result, ATOM_NUMBER) += 1.0f;
        current = ATOM_VALUE(current, ATOM_CONS).next;
    }

    return result;
}

Sexp Sexp_Concat(Environment *alloc, Sexp sexp) {
    return ATOM_MAKE(alloc, ATOM_NIL);
}

Sexp Sexp_Eval(Environment *alloc, Sexp sexp) {
    Sexp arg = Sexp_First(alloc, sexp);

    return Eval_Atom(alloc, ATOM_VALUE(arg, ATOM_QUOTE));
}

Sexp Sexp_Import(Environment *alloc, Sexp sexp) {
    Sexp arg = Sexp_First(alloc, sexp);

    if(ATOM_TY(arg) == ATOM_STRING) {
        Warlock_run_file(alloc, ATOM_VALUE(arg, ATOM_STRING));
    }
    
    return ATOM_MAKE(alloc, ATOM_NIL);
}

Sexp Sexp_If(Environment *alloc, Sexp sexp) {
    Sexp cond = Eval_Atom(alloc, Sexp_First(alloc, sexp));
    if(ATOM_TY(cond) != ATOM_BOOLEAN) {
        Warlock_error("condition must be a boolean");
        return ATOM_MAKE_NIL(alloc);
    }

    sexp = Sexp_Rest(alloc, sexp);
    Sexp t = Sexp_First(alloc, sexp);
    sexp = Sexp_Rest(alloc, sexp);
    Sexp f = Sexp_First(alloc, sexp);

    return Eval_Atom(alloc, ATOM_VALUE(cond, ATOM_BOOLEAN) ? t : f);
}
