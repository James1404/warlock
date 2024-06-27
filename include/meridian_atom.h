#ifndef MERIDIAN_ATOM_H
#define MERIDIAN_ATOM_H

#include "meridian_common.h"
#include "meridian_string.h"

typedef struct Atom Atom;

typedef enum {
    INTEGER_WIDTH_ANY,

    INTEGER_WIDTH8,
    INTEGER_WIDTH16,
    INTEGER_WIDTH32,
    INTEGER_WIDTH64,
} IntegerWidth;

typedef enum {
    INTEGER_SIGN_ANY,

    INTEGER_UNSIGNED,
    INTEGER_SIGNED,
} IntegerSign;

typedef enum {
    REAL_WIDTH_ANY,

    REAL_WIDTH32,
    REAL_WIDTH64,
} RealWidth;

typedef struct {
    enum {
        TYPE_UNKNOWN,

        TYPE_UNIT,

        TYPE_BOOLEAN,

        TYPE_INTEGER,
        TYPE_REAL,

        TYPE_STRING,
        TYPE_FN,

        TYPE_LIST,

        TYPE_ANY,
    } type;

    union {
        struct {
            IntegerWidth width;
            IntegerSign sign;
        } integer;
        struct {
            RealWidth width;
        } real;
        struct {
            Atom* args;
            u64 args_len;

            Atom* body;
        } fn;
    } extra;
} Type;

#define TYPE_UNKNOWN() ((Type) { .type = TYPE_UNKOWN })
#define TYPE_UNIT() ((Type) { .type = TYPE_UNIT })
#define TYPE_BOOLEAN() ((Type) { .type = TYPE_BOOLEAN })

#define TYPE_INTEGER(w, s) ((Type) { .type = TYPE_INTEGER, .extra.integer = { .width = w, .sign = s } })
#define TYPE_INTEGER_ANY() ((Type) { .type = TYPE_INTEGER, .extra.integer = { .width = INTEGER_WIDTH_ANY, .sign = INTEGER_SIGN_ANY } })

#define TYPE_REAL(w) ((Type) { .type = TYPE_REAL, .extra.real = { .width = w } })
#define TYPE_REAL_ANY() ((Type) { .type = TYPE_REAL, .extra.real = { .width = REAL_WIDTH_ANY } })


#define TYPE_STRING() ((Type) { .type = TYPE_STRING })
#define TYPE_FN() ((Type) { .type = TYPE_FN, .extra.fn = {} }) // todo
#define TYPE_LIST() ((Type) { .type = TYPE_LIST })

#define TYPE_ANY() ((Type) { .type = TYPE_ANY })

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

    Type ret;

    i64 argc;
    Type* args;
} Intrinsic;

Intrinsic Intrinsic_make(const char* name, IntrinsicFn fn, Type ret);
Intrinsic Intrinsic_make_variadic(const char* name, IntrinsicFn fn, Type ret, Type argtype);

void Intrinsic_free(Intrinsic* intrinsic);

void Intrinsic_add_argument(Intrinsic* intrinsic, Type type);

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

    Type type;
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

#define ATOM_INTRINSIC(__name, __fn, __ret) ((Atom) { .ty = ATOM_INTRINSIC, .as.intrinsic = Intrinsic_make(__name, __fn, __ret) })

#define ATOM_INTRINSIC_VARIADIC(__name, __fn, __ret, __argtype) ((Atom) { .ty = ATOM_INTRINSIC, .as.intrinsic = Intrinsic_make_variadic(__name, __fn, __ret, __argtype) })

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
