#ifndef MERIDIAN_ATOM_H
#define MERIDIAN_ATOM_H

#include "meridian_common.h"

typedef struct Atom Atom;

typedef struct {
    Atom *left, *right;
} Cons;

typedef struct {
    String name;
} Symbol;

typedef struct {
    Cons args;
    struct Atom* body;
} Fn;

#define FOR_PRIMITIVES(DO)\
    DO(f64) DO(bool) DO(String)\
    DO(Cons) DO(Symbol)\
    DO(Fn)

#define ATOM_GET_VALUE(x) ATOM_VALUE_##x
#define ATOM_GET_TYPE(x) ATOM_TYPE_##x

#define DEFINE_ATOM_VALUE(x) x ATOM_GET_VALUE(x);
#define DEFINE_ATOM_TYPE(x) ATOM_GET_TYPE(x),

struct Atom {
    union { FOR_PRIMITIVES(DEFINE_ATOM_VALUE) } value;
    enum { FOR_PRIMITIVES(DEFINE_ATOM_TYPE) } type;
};

typedef struct ListNode {
    struct ListNode* next;
    Atom atom;
} ListNode;

String Atom_toString(Atom* atom);

#define DEFINE_ATOM_MAKE(x)\
    Atom Atom_make_##x(x value);

#define DEFINE_ATOM_MAKE_IMPL(x)\
    Atom Atom_make_##x(x value) {\
        return (Atom) {\
            .type = ATOM_GET_TYPE(x),\
            .value.ATOM_GET_VALUE(x) = value\
        };\
    }

FOR_PRIMITIVES(DEFINE_ATOM_MAKE)

#endif//MERIDIAN_ATOM_H
