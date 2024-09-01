#ifndef WARLOCK_ATOM_H
#define WARLOCK_ATOM_H

#include "warlock_common.h"
#include "warlock_string.h"

typedef u64 Sexp;
typedef struct SexpAllocator SexpAllocator;

typedef struct {
    Sexp args;
    Sexp body;
} Fn;

typedef Sexp (*IntrinsicFn)(SexpAllocator* alloc, Sexp args);
typedef struct {
    const char* name;
    IntrinsicFn fn;
    i64 argc;
} Intrinsic;

typedef struct {
    void* ptr;
} FFI_Func;

typedef struct {
    Sexp args, body;
} Syntax;

typedef enum {
  ATOM_NUMBER,
  ATOM_BOOLEAN,
  ATOM_STRING,
  ATOM_SYMBOL,
  ATOM_KEYWORD,
  ATOM_FN,
  ATOM_INTRINSIC,
  ATOM_FFI,
  ATOM_CONS,
  ATOM_QUOTE,
  ATOM_NIL,
  ATOM_SYNTAX,
} AtomType;

typedef struct {
    Sexp data, next;
} Cons;

typedef struct {
    AtomType ty;
    union {
        f64 ATOM_NUMBER;
        bool ATOM_BOOLEAN;
        String ATOM_STRING;
        String ATOM_SYMBOL;
        String ATOM_KEYWORD;
        Fn ATOM_FN;
        Cons ATOM_CONS;
        Intrinsic ATOM_INTRINSIC;
        FFI_Func ATOM_FFI;
        Sexp ATOM_QUOTE;
        Syntax ATOM_SYNTAX;
    } as;
} Atom;

typedef struct {
    u64 scope;
    String name;
    Sexp sexp;
} Local;

struct SexpAllocator {
    u64 len, allocated;
    Atom* data;
    
    u64 scope;

    Sexp nil;

    u64 localsLen, localsAllocated;
    Local* locals;
};

SexpAllocator SexpAllocator_make(void);
void SexpAllocator_free(SexpAllocator* alloc);

Sexp SexpAllocator_alloc(SexpAllocator *alloc, Atom atom);

void SexpAllocator_incScope(SexpAllocator *alloc);
void SexpAllocator_decScope(SexpAllocator *alloc);

void SexpAllocator_setLocal(SexpAllocator *alloc, String name, Sexp sexp);
Sexp SexpAllocator_getLocal(SexpAllocator *alloc, String name);

String SexpAllocator_toString(SexpAllocator *alloc, Sexp node);
void SexpAllocator_print(SexpAllocator *alloc, Sexp sexp);

bool SexpAllocator_ConsTerminated(SexpAllocator *alloc, Sexp sexp);

#define ATOM_GET(allocator, idx) ((allocator)->data + idx)

#define ATOM_SET(allocator, idx, ty)                                           \
  *ATOM_GET(alloc, idx) = (Atom) {                                             \
    ty, { 0 }                                                                  \
  }
#define ATOM_SET_V(allocator, idx, ty, value)                                  \
  *ATOM_GET(alloc, idx) = (Atom) {                                             \
    ty, { .ty = value }                                                        \
  }
#define ATOM_SET_S(allocator, idx, ty, ...)                                    \
  *ATOM_GET(alloc, idx) = (Atom) {                                             \
    ty, {                                                                      \
      .ty = { __VA_ARGS__ }                                                    \
    }                                                                          \
  }

#define ATOM_TY(allocator, idx) (ATOM_GET((allocator), idx)->ty)
#define ATOM_VALUE(allocator, idx, ty) (ATOM_GET((allocator), idx)->as.ty)

#define ATOM_MAKE(allocator, ty)                                               \
  (SexpAllocator_alloc((allocator), (Atom){ty, {0}}))
#define ATOM_MAKE_V(allocator, ty, value)                                      \
  (SexpAllocator_alloc((allocator), (Atom){ty, {.ty = value}}))
#define ATOM_MAKE_S(allocator, ty, ...)                                        \
  (SexpAllocator_alloc((allocator), (Atom){ty, {.ty = {__VA_ARGS__}}}))

#define ATOM_MAKE_NIL(allocator) (ATOM_MAKE(allocator, ATOM_NIL))
#define ATOM_MAKE_CONS(allocator)                                              \
  (ATOM_MAKE_S(allocator, ATOM_CONS, ATOM_MAKE_NIL(allocator),                 \
               ATOM_MAKE_NIL(allocator)))

#define INTRINSIC(alloc, name, fn, argc)                                \
    do {                                                                \
    Sexp sexp = ATOM_MAKE_S(alloc, ATOM_INTRINSIC, name, &fn, argc);    \
    SexpAllocator_setLocal(alloc, STR(name), sexp);                     \
    } while(0);

#define GLOBAL(alloc, name, value) SexpAllocator_setLocal(alloc, STR(name), value)

#endif//WARLOCK_ATOM_H
