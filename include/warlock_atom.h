#ifndef WARLOCK_ATOM_H
#define WARLOCK_ATOM_H

#include "warlock_arena.h"
#include "warlock_common.h"
#include "warlock_string.h"

typedef struct Atom* Sexp;
typedef struct Environment Environment;

typedef struct Fn {
    Sexp args;
    Sexp body;
} Fn;

typedef Sexp (*IntrinsicFn)(Environment* env, Sexp args);
typedef struct Intrinsic {
    const char* name;
    IntrinsicFn fn;
    i64 argc;
    bool eval_all;
} Intrinsic;

typedef struct FFI_Func {
    void* ptr;
} FFI_Func;

typedef enum AtomType {
    ATOM_NUMBER = 0,
    ATOM_BOOLEAN,
    ATOM_STRING,
    ATOM_SYMBOL,
    ATOM_KEYWORD,
    ATOM_FN,
    ATOM_INTRINSIC,
    ATOM_FFI,
    ATOM_CONS,
    ATOM_LIST,
    ATOM_QUOTE,
    ATOM_NIL,
} AtomType;

typedef struct Cons {
    Sexp data, next;
} Cons;

typedef struct List {
    Sexp* data;
    usize capacity, len;
} List;

List List_make(void);
void List_free(List* list);

void List_push(List* list, Sexp sexp);
Sexp List_pop(List* list);

typedef struct Atom {
    AtomType ty;
    union {
        f64 ATOM_NUMBER;
        bool ATOM_BOOLEAN;
        String ATOM_STRING;
        String ATOM_SYMBOL;
        String ATOM_KEYWORD;
        Fn ATOM_FN;
        Cons ATOM_CONS;
        List ATOM_LIST;
        Intrinsic ATOM_INTRINSIC;
        FFI_Func ATOM_FFI;
        Sexp ATOM_QUOTE;
    } as;
} Atom;

typedef struct Local {
    u64 scope;
    String name;
    Sexp sexp;
} Local;

struct Environment {
    Arena arena;

    u64 scope;

    u64 localsLen, localsAllocated;
    Local* locals;
};

Environment Environment_make(void);
void Environment_free(Environment* env);

Sexp Environment_alloc(Environment* env, Atom atom);

void Environment_incScope(Environment* env);
void Environment_decScope(Environment* env);

void Environment_setLocal(Environment* env, String name, Sexp sexp);
Sexp Environment_getLocal(Environment* env, String name);

String Environment_toString(Environment* env, Sexp node);
void Environment_print(Environment* env, Sexp sexp);

bool Environment_ConsTerminated(Environment* env, Sexp sexp);
u64 Environment_ConsLen(Environment* env, Sexp sexp);

#define ATOM_SET(sexp, ty)                                                     \
    *(sexp) = (Atom) {                                                         \
        ty, { 0 }                                                              \
    }
#define ATOM_SET_V(sexp, ty, value)                                            \
    *(sexp) = (Atom) {                                                         \
        ty, { .ty = value }                                                    \
    }
#define ATOM_SET_S(sexp, ty, ...)                                              \
    *(sexp) = (Atom) {                                                         \
        ty, {                                                                  \
            .ty = { __VA_ARGS__ }                                              \
        }                                                                      \
    }

#define ATOM_TY(sexp) ((sexp)->ty)
#define ATOM_VALUE(sexp, ty) ((sexp)->as.ty)

#define ATOM_MAKE(env, ty) (Environment_alloc((env), (Atom){ty, {0}}))
#define ATOM_MAKE_V(env, ty, value)                                            \
    (Environment_alloc((env), (Atom){ty, {.ty = value}}))
#define ATOM_MAKE_S(env, ty, ...)                                              \
    (Environment_alloc((env), (Atom){ty, {.ty = {__VA_ARGS__}}}))

#define ATOM_MAKE_NIL(env) (ATOM_MAKE(env, ATOM_NIL))
#define ATOM_MAKE_CONS(env)                                                    \
    (ATOM_MAKE_S(env, ATOM_CONS, ATOM_MAKE_NIL(env), ATOM_MAKE_NIL(env)))
#define ATOM_MAKE_LIST(env) (ATOM_MAKE_V(env, ATOM_LIST, List_make()))

#define INTRINSIC(env, name, fn, argc, eval_all)                               \
    do {                                                                       \
        Sexp sexp =                                                            \
            ATOM_MAKE_S(env, ATOM_INTRINSIC, name, &fn, argc, eval_all);       \
        Environment_setLocal(env, STR(name), sexp);                            \
    } while (0);

#define GLOBAL(env, name, value) Environment_setLocal(env, STR(name), value)

#endif // WARLOCK_ATOM_H
