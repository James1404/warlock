#ifndef MERIDIAN_ATOM_H
#define MERIDIAN_ATOM_H

#include "meridian_common.h"

typedef struct Atom Atom;

typedef struct {
    u64 length, allocated;
    Atom* data;
} List;

List List_make();
void List_free(List* list);

void List_push(List* list, Atom atom);
Atom List_at(List* list, u64 index);

typedef struct {
    u64 args_length, args_allocated;
    String* args;

    Atom* body;
} Fn;

Fn Fn_make();
void Fn_free(Fn* fn);
void Fn_push(Fn* fn, String arg);

typedef enum {
    ATOM_NUMBER,
    ATOM_BOOLEAN,
    ATOM_STRING,
    ATOM_SYMBOL,
    ATOM_KEYWORD,
    ATOM_FN,
    ATOM_LIST,
    ATOM_NIL,
} AtomType;

struct Atom {
    AtomType ty;
    union {
        f64 number;
        bool boolean;
        String string;
        Fn fn;
        List list;
    } as;
};

#define ATOM_NUMBER(value) ((Atom) { .ty = ATOM_NUMBER, .as.number = value  })
#define ATOM_BOOL(value) ((Atom) { .ty = ATOM_BOOLEAN, .as.number = value  })
#define ATOM_STRING(value) ((Atom) { .ty = ATOM_STRING, .as.string = value  })
#define ATOM_SYMBOL(value) ((Atom) { .ty = ATOM_SYMBOL, .as.string = value  })
#define ATOM_KEYWORD(value) ((Atom) { .ty = ATOM_KEYWORD, .as.string = value  })
#define ATOM_FN() ((Atom) { .ty = ATOM_FN, .as.fn = Fn_make() })
#define ATOM_LIST() ((Atom) { .ty = ATOM_LIST, .as.list = List_make()  })
#define ATOM_NIL() ((Atom) { .ty = ATOM_NIL })

#define GET_ATOM_NUMBER(atom) ((atom).as.number)
#define GET_ATOM_BOOL(atom) ((atom).as.boolean)
#define GET_ATOM_STRING(atom) ((atom).as.string)
#define GET_ATOM_SYMBOL(atom) ((atom).as.string)
#define GET_ATOM_KEYWORD(atom) ((atom).as.string)
#define GET_ATOM_FN(atom) ((atom).as.fn)
#define GET_ATOM_LIST(atom) ((atom).as.list)

#endif//MERIDIAN_ATOM_H
