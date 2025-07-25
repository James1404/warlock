#include "warlock_atom.h"

#include "warlock_arena.h"
#include "warlock_builtins.h"
#include "warlock_error.h"
#include "warlock_string.h"

#include <stdio.h>
#include <stdlib.h>

#define PRINT_WHERE() printf("AT := %s\n", __func__)

List List_make(void) {
    return (List){
        .data = NULL,
        .capacity = 0,
        .len = 0,
    };
}
void List_free(List* list) {
    free(list->data);
    list->data = NULL;

    list->capacity = 0;
    list->len = 0;
}

void List_push(List* list, Sexp sexp) {
    if (!list->data) {
        list->capacity = 8;
        list->len = 0;
        list->data = malloc(sizeof(Sexp) * list->capacity);
    }

    if (list->len == list->capacity) {
        list->capacity *= 2;
        list->data = malloc(sizeof(Sexp) * list->capacity);
    }

    list->data[list->len++] = sexp;
}

Sexp List_pop(List* list) { return list->data[list->len--]; }

Environment Environment_make(void) {
    Environment result = {
        .arena = Arena_make(),

        .localsLen = 0,
        .localsAllocated = 8,
        .locals = NULL,
    };

    return result;
}

void Environment_free(Environment* alloc) {
    Arena_free(&alloc->arena);
    free(alloc->locals);
}

Sexp Environment_alloc(Environment* alloc, Atom atom) {
    Sexp ptr = malloc(sizeof(Atom));
    *ptr = atom;
    return ptr;
}

void Environment_incScope(Environment* alloc) { alloc->scope++; }
void Environment_decScope(Environment* alloc) {
    alloc->scope--;

    while (alloc->locals[alloc->localsLen - 1].scope > alloc->scope) {
        alloc->localsLen--;
    }
}

void Environment_setLocal(Environment* alloc, String name, Sexp sexp) {
    if (!alloc->locals) {
        alloc->localsLen = 0;
        alloc->localsAllocated = 8;
        alloc->locals = malloc(sizeof(Local) * alloc->localsAllocated);
    }

    if (alloc->localsLen >= alloc->localsAllocated) {
        alloc->localsAllocated *= 2;
        alloc->locals =
            realloc(alloc->locals, sizeof(Local) * alloc->localsAllocated);
    }

    Local* local = alloc->locals + (alloc->localsLen++);
    *local = (Local){
        .scope = alloc->scope,
        .sexp = sexp,
        .name = name,
    };
}

Sexp Environment_getLocal(Environment* alloc, String name) {
    for (i64 i = alloc->localsLen - 1; i >= 0; i--) {
        Local entry = alloc->locals[i];

        if (STR_CMP(entry.name, name)) {
            return entry.sexp;
        }
    }

    Warlock_error("Could not find symbol '%.*s' in type environment", name.len,
                  name.raw);
    return ATOM_MAKE(alloc, ATOM_NIL);
}

String Environment_toString(Environment* alloc, Sexp node) {
    switch (ATOM_TY(node)) {
    case ATOM_NUMBER:
        return STR("NUMBER");
    case ATOM_BOOLEAN:
        return STR("BOOLEAN");
    case ATOM_STRING:
        return STR("STRING");
    case ATOM_SYMBOL:
        return STR("SYMBOL");
    case ATOM_KEYWORD:
        return STR("KEYWORD");
    case ATOM_FN:
        return STR("FN");
    case ATOM_INTRINSIC:
        return STR("INTRINSIC");
    case ATOM_FFI:
        return STR("FFI");
    case ATOM_CONS:
        return STR("CONS");
    case ATOM_LIST:
        return STR("LIST");
    case ATOM_QUOTE:
        return STR("QUOTE");
    case ATOM_NIL:
        return STR("NIL");
    }

    return STR("Invalid String");
}

void Environment_print(Environment* alloc, Sexp sexp) {
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
        printf("\"%.*s\"", v.len, v.raw);
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

        Environment_print(alloc, fn.args);

        printf(" -> ");

        Environment_print(alloc, fn.body);

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
            Sexp head = Sexp_First(alloc, current);

            Environment_print(alloc, head);

            printf(" ");

            current = Sexp_Rest(alloc, current);
        }

        printf(")");
    } break;
    case ATOM_LIST: {
        List list = ATOM_VALUE(sexp, ATOM_LIST);

        printf("(");

        for (i32 i = 0; i < list.len; i++) {
            Sexp value = list.data[i];
            Environment_print(alloc, value);
        }

        printf(")");
    } break;

    case ATOM_QUOTE: {
        printf("'");
        Environment_print(alloc, ATOM_VALUE(sexp, ATOM_QUOTE));
    } break;
    case ATOM_NIL: {
        printf("nil");
    } break;
    }
}

bool Environment_ConsTerminated(Environment* alloc, Sexp sexp) {
    return ATOM_TY(sexp) == ATOM_NIL;
}

u64 Environment_ConsLen(Environment* alloc, Sexp sexp) {
    u64 result = 0;

    Sexp current = sexp;

    while (!Environment_ConsTerminated(alloc, current)) {
        result++;
        current = ATOM_VALUE(current, ATOM_CONS).next;
    }

    return result;
}
