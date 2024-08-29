#include "warlock_atom.h"

#include "warlock_builtins.h"
#include "warlock_error.h"
#include "warlock_string.h"

#include <stdlib.h>
#include <stdio.h>

#define PRINT_WHERE() printf("AT := %s\n", __func__)

SexpAllocator SexpAllocator_make(void) {
    SexpAllocator result = {
        .len = 0,
        .allocated = 0,
        .data = NULL,

        .localsLen = 0,
        .localsAllocated = 0,
        .locals = NULL,

        .nil = 0,
    };

    result.nil = ATOM_MAKE(&result, ATOM_NIL);

    return result;
}

void SexpAllocator_free(SexpAllocator* alloc) {
    free(alloc->data);
    free(alloc->locals);
}

Sexp SexpAllocator_alloc(SexpAllocator* alloc, Atom atom) {
    if(!alloc->data) {
        alloc->allocated = 100;
        alloc->data = malloc(sizeof(Atom)*alloc->allocated);
    }

    Sexp sexp = alloc->len;
    alloc->data[sexp] = atom;
    alloc->len++;

    if(alloc->len >= alloc->allocated) {
        alloc->allocated *= 2;
        Atom* temp = realloc(alloc->data, sizeof(Atom)*alloc->allocated);
        if(temp) {
            alloc->data = temp;
        }
        else {
            Warlock_error("Failed to realloc in %s", __func__);
        }
    }

    return sexp;
}


void SexpAllocator_incScope(SexpAllocator *alloc) { alloc->scope++; }
void SexpAllocator_decScope(SexpAllocator *alloc) {
    alloc->scope--;

    while(alloc->locals[alloc->localsLen - 1].scope > alloc->scope) {
        alloc->localsLen--;
    }
}

void SexpAllocator_setLocal(SexpAllocator* alloc, String name, Sexp sexp) {
    if(!alloc->locals) {
        alloc->localsLen = 0;
        alloc->localsAllocated = 8;
        alloc->locals = malloc(sizeof(Local) * alloc->localsAllocated);
    }

    if(alloc->localsLen >= alloc->localsAllocated) {
        alloc->localsAllocated *= 2;

        Local* temp = realloc(alloc->locals, sizeof(Local) * alloc->localsAllocated);
        if(temp) {
            alloc->locals = temp;
        }
        else {
            Warlock_error("Env_set realloc error");
            return;
        }
    }

    Local* local = alloc->locals + (alloc->localsLen++);
    *local = (Local) {
        .scope = alloc->scope,
        .sexp = sexp,
    };

    STR_CPY_ALLOC(local->name, name);
}

Sexp SexpAllocator_getLocal(SexpAllocator* alloc, String name) {
    for(i64 i = alloc->localsLen - 1; i >= 0; i--) {
        Local entry = alloc->locals[i];

        if(STR_CMP(entry.name, name)) {
            return entry.sexp;
        }
    }

        Warlock_error("Could not find symbol '%.*s' in type environment", name.len, name.raw);
    return ATOM_MAKE(alloc, ATOM_NIL);
}

String SexpAllocator_toString(SexpAllocator* alloc, Sexp node) {
    switch(ATOM_TY(alloc, node)) {
    case ATOM_NUMBER: return STR("NUMBER");
    case ATOM_BOOLEAN: return STR("BOOLEAN");
    case ATOM_STRING: return STR("STRING");
    case ATOM_SYMBOL: return STR("SYMBOL");
    case ATOM_KEYWORD: return STR("KEYWORD");
    case ATOM_FN: return STR("FN");
    case ATOM_INTRINSIC: return STR("INTRINSIC");
    case ATOM_FFI: return STR("FFI");
    case ATOM_CONS: return STR("CONS");
    case ATOM_QUOTE: return STR("QUOTE");
    case ATOM_NIL: return STR("NIL");
    }

    return STR("Invalid String");
}

void SexpAllocator_print(SexpAllocator *alloc, Sexp sexp) {
    switch(ATOM_TY(alloc, sexp)) {
    case ATOM_NUMBER: {
        printf("%Lf", ATOM_VALUE(alloc, sexp, ATOM_NUMBER));
    } break;
    case ATOM_BOOLEAN: {
        bool v = ATOM_VALUE(alloc, sexp, ATOM_NUMBER);
        printf("%s", v ? "#t" : "#f");
    } break;
    case ATOM_STRING: {
        String v = ATOM_VALUE(alloc, sexp, ATOM_STRING);
        printf("\"%.*s\"", v.len, v.raw);
    } break;
    case ATOM_SYMBOL: {
        String v = ATOM_VALUE(alloc, sexp, ATOM_SYMBOL);
        printf("%.*s", v.len, v.raw);
    } break;
    case ATOM_KEYWORD: {
        String v = ATOM_VALUE(alloc, sexp, ATOM_KEYWORD);
        printf(":%.*s", v.len, v.raw);
    } break;
    case ATOM_FN: {
        Fn fn = ATOM_VALUE(alloc, sexp, ATOM_FN);

        printf("(");

        printf("fn ");

        SexpAllocator_print(alloc, fn.args);

        printf(" -> ");

        SexpAllocator_print(alloc, fn.body);
        
        printf(")");
    } break;
    case ATOM_INTRINSIC: {
        printf("#intrinsic");
    } break;
    case ATOM_FFI: {
        printf("#ffi");
    } break;
    case ATOM_CONS: {
        printf("(");
        
        Sexp current = sexp;
        
        while(ATOM_TY(alloc, sexp) != ATOM_NIL) {
            Sexp head = Sexp_First(alloc, current);

            SexpAllocator_print(alloc, head);

            current = Sexp_Rest(alloc, current);
        }

        printf(")");
    } break;
    case ATOM_QUOTE: {
        printf("'");
        SexpAllocator_print(alloc, ATOM_VALUE(alloc, sexp, ATOM_QUOTE));
    } break;
    case ATOM_NIL: {
        printf("nil");
    } break;
    }
}

bool SexpAllocator_ConsTerminated(SexpAllocator *alloc, Sexp sexp) {
    return ATOM_TY(alloc, sexp) == ATOM_NIL;
}
