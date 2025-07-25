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

typedef Sexp (*IntrinsicFn)(Environment* alloc, Sexp args);
typedef struct Intrinsic {
    const char* name;
    IntrinsicFn fn;
    i64 argc;
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
void Environment_free(Environment* alloc);

Sexp Environment_alloc(Environment* alloc, Atom atom);

void Environment_incScope(Environment* alloc);
void Environment_decScope(Environment* alloc);

void Environment_setLocal(Environment* alloc, String name, Sexp sexp);
Sexp Environment_getLocal(Environment* alloc, String name);

String Environment_toString(Environment* alloc, Sexp node);
void Environment_print(Environment* alloc, Sexp sexp);

bool Environment_ConsTerminated(Environment* alloc, Sexp sexp);
u64 Environment_ConsLen(Environment* alloc, Sexp sexp);

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

#define ATOM_MAKE(allocator, ty)                                               \
    (Environment_alloc((allocator), (Atom){ty, {0}}))
#define ATOM_MAKE_V(allocator, ty, value)                                      \
    (Environment_alloc((allocator), (Atom){ty, {.ty = value}}))
#define ATOM_MAKE_S(allocator, ty, ...)                                        \
    (Environment_alloc((allocator), (Atom){ty, {.ty = {__VA_ARGS__}}}))

#define ATOM_MAKE_NIL(allocator) (ATOM_MAKE(allocator, ATOM_NIL))
#define ATOM_MAKE_CONS(allocator)                                              \
    (ATOM_MAKE_S(allocator, ATOM_CONS, ATOM_MAKE_NIL(allocator),               \
                 ATOM_MAKE_NIL(allocator)))
#define ATOM_MAKE_LIST(allocator)                                              \
    (ATOM_MAKE_V(allocator, ATOM_LIST, List_make()))

#define INTRINSIC(alloc, name, fn, argc)                                       \
    do {                                                                       \
        Sexp sexp = ATOM_MAKE_S(alloc, ATOM_INTRINSIC, name, &fn, argc);       \
        Environment_setLocal(alloc, STR(name), sexp);                        \
    } while (0);

#define GLOBAL(alloc, name, value)                                             \
    Environment_setLocal(alloc, STR(name), value)

#endif // WARLOCK_ATOM_H
