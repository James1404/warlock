#ifndef MERIDIAN_TYPES_H
#define MERIDIAN_TYPES_H

#include "meridian_common.h"
#include "meridian_hash.h"
#include "meridian_string.h"

typedef u64 TypeVar;

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
    IntegerWidth width;
    IntegerSign sign;
} IntegerTypeData;

typedef struct {
    RealWidth width;
} RealTypeData;

typedef struct {
    struct Type* ret;

    i64 argc;
    struct Type* args;
} FnTypeData;

typedef struct {
    struct Type* items;
    u32 len, allocated;
} ListTypeData;

typedef struct Type {
    enum {
        TYPE_UNKNOWN,

        TYPE_VAR,

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
        TypeVar typevar;
        IntegerTypeData integer;
        RealTypeData real;
        FnTypeData fn;
        ListTypeData list;
    } extra;
} Type;

#define TYPE_UNKNOWN() ((Type) { .type = TYPE_UNKNOWN })

#define TYPE_VAR(__id) ((Type) { .type = TYPE_UNKNOWN, .extra.typevar = __id })

#define TYPE_UNIT() ((Type) { .type = TYPE_UNIT })
#define TYPE_BOOLEAN() ((Type) { .type = TYPE_BOOLEAN })

#define TYPE_INTEGER(w, s) ((Type) { .type = TYPE_INTEGER, .extra.integer = { .width = w, .sign = s } })
#define TYPE_INTEGER_ANY() ((Type) { .type = TYPE_INTEGER, .extra.integer = { .width = INTEGER_WIDTH_ANY, .sign = INTEGER_SIGN_ANY } })

#define TYPE_REAL(w) ((Type) { .type = TYPE_REAL, .extra.real = { .width = w } })
#define TYPE_REAL_ANY() ((Type) { .type = TYPE_REAL, .extra.real = { .width = REAL_WIDTH_ANY } })

#define TYPE_STRING() ((Type) { .type = TYPE_STRING })
#define TYPE_FN(__ret) ((Type) { .type = TYPE_FN, .extra.fn = { 0 } }) // todo
#define TYPE_LIST() ((Type) { .type = TYPE_LIST })

#define TYPE_ANY() ((Type) { .type = TYPE_ANY })

#define GET_TYPE_VAR(type) ((type).extra.typevar)
#define GET_TYPE_INTEGER(type) ((type).extra.integer)
#define GET_TYPE_REAL(type) ((type).extra.real)
#define GET_TYPE_FN(type) ((type).extra.fn)
#define GET_TYPE_LIST(type) ((type).extra.list)

void Type_add_arg(Type* type, Type arg);

typedef struct {
    u64 scope;
    String name;
    Type type;
} TypeLocal;

typedef struct {
    u64 scope;

    u64 length, allocated;
    TypeLocal* locals;
} TypeEnv;

void TypeEnv_init(void);
void TypeEnv_free(void);

void TypeEnv_inc(void);
void TypeEnv_dec(void);

Type TypeEnv_get(String name);
void TypeEnv_set(String name, Type type);

#endif//MERIDIAN_TYPES_H
