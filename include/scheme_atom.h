#ifndef MERIDIAN_ATOM_H
#define MERIDIAN_ATOM_H

#include "scheme_common.h"
#include "scheme_string.h"

typedef struct Atom Atom;

typedef struct {
    u64 length, allocated;
    Atom* data;
} List;

List List_make(void);
void List_free(List* list);

void List_push(List* list, Atom atom);
Atom List_at(List* list, u64 index);

typedef struct {
    u64 args_length, args_allocated;
    String* args;

    Atom* body;
} Fn;

Fn Fn_make(void);
void Fn_free(Fn* fn);
void Fn_push(Fn* fn, String arg);

typedef Atom (*IntrinsicFn)(List args);
typedef struct {
    IntrinsicFn fn;

    const char* name;

    i64 argc;
} Intrinsic;

typedef struct {
    void* ptr;
} FFI_Func;

typedef enum {
    ATOM_REAL,
    ATOM_INTEGER,
    ATOM_BOOLEAN,
    ATOM_STRING,
    ATOM_SYMBOL,
    ATOM_KEYWORD,
    ATOM_FN,
    ATOM_INTRINSIC,
    ATOM_FFI,
    ATOM_LIST,
    ATOM_QUOTE,
    ATOM_NIL,
} AtomType;

struct Atom {
    AtomType ty;
    union {
        f64 real;
        i64 integer;
        bool boolean;
        String string;
        Fn fn;
        List list;
        Intrinsic intrinsic;
        FFI_Func ffi;
        Atom* quote;
    } as;
};

//
// --- Builders ---
//
#define ATOM_REAL(value) ((Atom) { .ty = ATOM_REAL, .as.real = value  })
#define ATOM_INTEGER(value) ((Atom) { .ty = ATOM_INTEGER, .as.integer = value  })
#define ATOM_BOOLEAN(value) ((Atom) { .ty = ATOM_BOOLEAN, .as.boolean = value  })
#define ATOM_STRING(value) ((Atom) { .ty = ATOM_STRING, .as.string = value  })
#define ATOM_SYMBOL(value) ((Atom) { .ty = ATOM_SYMBOL, .as.string = value  })
#define ATOM_KEYWORD(value) ((Atom) { .ty = ATOM_KEYWORD, .as.string = value  })
#define ATOM_FN() ((Atom) { .ty = ATOM_FN, .as.fn = Fn_make() })

#define ATOM_INTRINSIC(__name, __fn, __argc) ((Atom) { .ty = ATOM_INTRINSIC, .as.intrinsic = (Intrinsic) { .name = __name, .fn = __fn, .argc = __argc } })

#define ATOM_FFI() ((Atom) { .ty = ATOM_FFI })
#define ATOM_LIST() ((Atom) { .ty = ATOM_LIST, .as.list = List_make()  })
#define ATOM_QUOTE(atom) ((Atom) { .ty = ATOM_QUOTE, .as.quote = atom })
#define ATOM_NIL() ((Atom) { .ty = ATOM_NIL })

//
// --- Getters ---
//

#define GET_ATOM_REAL(atom) ((atom).as.real)
#define GET_ATOM_INTEGER(atom) ((atom).as.integer)
#define GET_ATOM_BOOLEAN(atom) ((atom).as.boolean)
#define GET_ATOM_STRING(atom) ((atom).as.string)
#define GET_ATOM_SYMBOL(atom) ((atom).as.string)
#define GET_ATOM_KEYWORD(atom) ((atom).as.string)
#define GET_ATOM_FN(atom) ((atom).as.fn)
#define GET_ATOM_INTRINSIC(atom) ((atom).as.intrinsic)
#define GET_ATOM_FFI(atom) ((atom).as.ffi)
#define GET_ATOM_LIST(atom) ((atom).as.list)
#define GET_ATOM_QUOTE(atom) ((atom).as.quote)

#endif//MERIDIAN_ATOM_H
