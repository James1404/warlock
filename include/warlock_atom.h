#ifndef WARLOCK_ATOM_H
#define WARLOCK_ATOM_H

#include "warlock_arena.h"
#include "warlock_common.h"
#include "warlock_string.h"

typedef struct Atom* Sexp;
typedef struct SexpAllocator SexpAllocator;

typedef struct Fn {
    Sexp args;
    Sexp body;
} Fn;

typedef Sexp (*IntrinsicFn)(SexpAllocator* alloc, Sexp args);
typedef struct Intrinsic {
    const char* name;
    IntrinsicFn fn;
    i64 argc;
} Intrinsic;

typedef struct FFI_Func {
    void* ptr;
} FFI_Func;

typedef enum AtomType {
    ATOM_NUMBER,
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

struct SexpAllocator {
    Arena arena;

    u64 scope;

    u64 localsLen, localsAllocated;
    Local* locals;
};

SexpAllocator SexpAllocator_make(void);
void SexpAllocator_free(SexpAllocator* alloc);

Sexp SexpAllocator_alloc(SexpAllocator* alloc, Atom atom);

void SexpAllocator_incScope(SexpAllocator* alloc);
void SexpAllocator_decScope(SexpAllocator* alloc);

void SexpAllocator_setLocal(SexpAllocator* alloc, String name, Sexp sexp);
Sexp SexpAllocator_getLocal(SexpAllocator* alloc, String name);

String SexpAllocator_toString(SexpAllocator* alloc, Sexp node);
void SexpAllocator_print(SexpAllocator* alloc, Sexp sexp);

bool SexpAllocator_ConsTerminated(SexpAllocator* alloc, Sexp sexp);
u64 SexpAllocator_ConsLen(SexpAllocator* alloc, Sexp sexp);

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
    (SexpAllocator_alloc((allocator), (Atom){ty, {0}}))
#define ATOM_MAKE_V(allocator, ty, value)                                      \
    (SexpAllocator_alloc((allocator), (Atom){ty, {.ty = value}}))
#define ATOM_MAKE_S(allocator, ty, ...)                                        \
    (SexpAllocator_alloc((allocator), (Atom){ty, {.ty = {__VA_ARGS__}}}))

#define ATOM_MAKE_NIL(allocator) (ATOM_MAKE(allocator, ATOM_NIL))
#define ATOM_MAKE_CONS(allocator)                                              \
    (ATOM_MAKE_S(allocator, ATOM_CONS, ATOM_MAKE_NIL(allocator),               \
                 ATOM_MAKE_NIL(allocator)))
#define ATOM_MAKE_LIST(allocator)                                              \
    (ATOM_MAKE_V(allocator, ATOM_LIST, List_make()))

#define INTRINSIC(alloc, name, fn, argc)                                       \
    do {                                                                       \
        Sexp sexp = ATOM_MAKE_S(alloc, ATOM_INTRINSIC, name, &fn, argc);       \
        SexpAllocator_setLocal(alloc, STR(name), sexp);                        \
    } while (0);

#define GLOBAL(alloc, name, value)                                             \
    SexpAllocator_setLocal(alloc, STR(name), value)

#endif // WARLOCK_ATOM_H
