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

void Atom_free(Atom* atom) {
    switch (atom->ty) {
    case ATOM_STRING:
        free(ATOM_VALUE(atom, ATOM_STRING));
        break;
    case ATOM_SYMBOL:
        free(ATOM_VALUE(atom, ATOM_STRING));
        break;
    case ATOM_KEYWORD:
        free(ATOM_VALUE(atom, ATOM_STRING));
        break;
    case ATOM_FN: {
        Fn fn = ATOM_VALUE(atom, ATOM_FN);

        Atom_free(fn.args);
        Atom_free(fn.body);
    } break;
    case ATOM_INTRINSIC: {
        Intrinsic intrinsic = ATOM_VALUE(atom, ATOM_INTRINSIC);

        free(intrinsic.name);
    } break;
    case ATOM_FFI:
        break;
    case ATOM_CONS: {
        Cons cons = ATOM_VALUE(atom, ATOM_CONS);
        Atom_free(cons.data);
        Atom_free(cons.next);
    } break;
    case ATOM_LIST: {
        List* list = &ATOM_VALUE(atom, ATOM_LIST);
        for (usize i = 0; i < list->len; i++) {
            Atom_free(list->data[i]);
        }
        List_free(list);
    } break;
    case ATOM_QUOTE:
        Atom_free(ATOM_VALUE(atom, ATOM_QUOTE));
        break;
    case ATOM_NUMBER:
    case ATOM_BOOLEAN:
    case ATOM_NIL:
        break;
    }
}

Environment Environment_make(void) {
    Environment result = {
        .arena = Arena_make(),

        .localsLen = 0,
        .localsAllocated = 8,
        .locals = NULL,
    };

    return result;
}

void Environment_free(Environment* env) {
    Arena_free(&env->arena);
    free(env->locals);
}

Sexp Environment_alloc(Environment* env, Atom atom) {
    Sexp ptr = Arena_malloc(&env->arena, sizeof(Atom));
    *ptr = atom;
    return ptr;
}

void Environment_incScope(Environment* env) { env->scope++; }
void Environment_decScope(Environment* env) {
    env->scope--;

    while (env->locals[env->localsLen - 1].scope > env->scope) {
        env->localsLen--;
    }
}

void Environment_setLocal(Environment* env, char* name, Sexp sexp) {
    if (!env->locals) {
        env->localsLen = 0;
        env->localsAllocated = 8;
        env->locals = malloc(sizeof(Local) * env->localsAllocated);
    }

    if (env->localsLen >= env->localsAllocated) {
        env->localsAllocated *= 2;
        env->locals =
            realloc(env->locals, sizeof(Local) * env->localsAllocated);
    }

    Local* local = env->locals + (env->localsLen++);
    *local = (Local){
        .scope = env->scope,
        .sexp = sexp,
        .name = name,
    };
}

Sexp Environment_getLocal(Environment* env, char* name) {
    for (i64 i = env->localsLen - 1; i >= 0; i--) {
        Local entry = env->locals[i];

        if (string_equal(entry.name, name)) {
            return entry.sexp;
        }
    }

    Warlock_error("Could not find symbol '%s' in environment", name);
    return ATOM_MAKE(env, ATOM_NIL);
}

void Environment_print(Environment* env, Sexp sexp) {
    switch (ATOM_TY(sexp)) {
    case ATOM_NUMBER: {
        printf("%Lf", ATOM_VALUE(sexp, ATOM_NUMBER));
    } break;
    case ATOM_BOOLEAN: {
        bool v = ATOM_VALUE(sexp, ATOM_NUMBER);
        printf("%s", v ? "#t" : "#f");
    } break;
    case ATOM_STRING: {
        printf("\"%s\"", ATOM_VALUE(sexp, ATOM_STRING));
    } break;
    case ATOM_SYMBOL: {
        printf("%s", ATOM_VALUE(sexp, ATOM_SYMBOL));
    } break;
    case ATOM_KEYWORD: {
        printf(":%s", ATOM_VALUE(sexp, ATOM_KEYWORD));
    } break;
    case ATOM_FN: {
        Fn fn = ATOM_VALUE(sexp, ATOM_FN);

        printf("(");

        printf("fn ");

        Environment_print(env, fn.args);

        printf(" -> ");

        Environment_print(env, fn.body);

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

            Environment_print(env, head);

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
            Environment_print(env, value);
        }

        printf(")");
    } break;

    case ATOM_QUOTE: {
        printf("'");
        Environment_print(env, ATOM_VALUE(sexp, ATOM_QUOTE));
    } break;
    case ATOM_NIL: {
        printf("nil");
    } break;
    }
}

bool Environment_ConsTerminated(Environment* env, Sexp sexp) {
    return ATOM_TY(sexp) == ATOM_NIL;
}

u64 Environment_ConsLen(Environment* env, Sexp sexp) {
    u64 result = 0;

    Sexp current = sexp;

    while (!Environment_ConsTerminated(env, current)) {
        result++;
        current = ATOM_VALUE(current, ATOM_CONS).next;
    }

    return result;
}
