#include "meridian_eval.h"
#include "meridian_error.h"

#include <stdlib.h>

Eval Eval_make() {
    Eval eval = {
        .env = Env_make(),
        .intrinsics = IntrinsicMap_make()
    };

    return eval;
}

void Eval_free(Eval* eval) {
    Env_free(&eval->env);
}

void Eval_run(Eval* eval, Atom atom) {
    Eval_Atom(eval, atom);
}

Atom Eval_Atom(Eval* eval, Atom atom) {
    switch(atom.ty) {
        case ATOM_SYMBOL: {
            return Env_get(&eval->env, GET_ATOM_SYMBOL(atom));
        } break;
        case ATOM_LIST: return Eval_List(eval, atom);
        case ATOM_FN: return Eval_Fn(eval, atom);

        case ATOM_NUMBER:
        case ATOM_BOOLEAN:
        case ATOM_STRING:
        case ATOM_NIL:
        case ATOM_KEYWORD:
            return atom;
    }

    return ATOM_NIL();
}

Atom Eval_List(Eval* eval, Atom atom) {
    Atom predicate = List_at(&GET_ATOM_LIST(atom), 0);

    if(predicate.ty == ATOM_SYMBOL) {
        if(String_is(GET_ATOM_SYMBOL(predicate), "fn")) {
            return Eval_Fn(eval, atom);
        }
    }

    for(u64 i = 0; i < GET_ATOM_LIST(atom).length; i++) {
        GET_ATOM_LIST(atom).data[i] = Eval_Atom(eval, GET_ATOM_LIST(atom).data[i]);
    }

    return predicate;
}

bool Eval_match(Eval* eval, Atom atom, const char* expected) {
    if(atom.ty == ATOM_SYMBOL) {
        String sym = GET_ATOM_SYMBOL(atom);
        return String_is(sym, expected);
    }

    return false;
}

Atom Eval_Def(Eval* eval, Atom atom) {
    if(atom.ty != ATOM_LIST) return ATOM_NIL();

    List list = GET_ATOM_LIST(atom);

    if(list.length != 3) {
        Meridian_error("invalid definition");
        return ATOM_NIL();
    }

    if(Eval_match(eval, list.data[0], "def")) {
        if(list.data[1].ty == ATOM_SYMBOL) {
            Env_set(&eval->env, GET_ATOM_SYMBOL(list.data[1]), list.data[2]);
        }
    }
    
    return ATOM_NIL();
}

Atom Eval_Fn(Eval* eval, Atom atom) {
    if(atom.ty != ATOM_LIST) return ATOM_NIL();
    if(!Eval_match(eval, GET_ATOM_LIST(atom).data[0], "fn")) return ATOM_NIL();

    Atom args = GET_ATOM_LIST(atom).data[1];
    Atom body = GET_ATOM_LIST(atom).data[2];
    if(args.ty != ATOM_LIST) return ATOM_NIL();

    Atom result = ATOM_FN();

    for(u64 i = 0; i < GET_ATOM_LIST(args).length; i++) {
        Atom arg = GET_ATOM_LIST(args).data[i];
        if(arg.ty != ATOM_SYMBOL) {
            Meridian_error("function parameters must all be symbols");
            return ATOM_NIL();
        }

        Fn_push(&GET_ATOM_FN(result), GET_ATOM_STRING(arg));
    }

    GET_ATOM_FN(result).body = malloc(sizeof(Atom));
    GET_ATOM_FN(result).body = memcpy(GET_ATOM_FN(result).body, &body, sizeof(Atom));
    
    return result;
}
