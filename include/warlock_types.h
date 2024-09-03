#ifndef WARLOCK_TYPES_H
#define WARLOCK_TYPES_H

#include "warlock_common.h"
typedef usize TypeIdx;

typedef struct {
    enum {
        TYPE_EXISTS,
        TYPE_FORALL,

        TYPE_NUMBER,
        TYPE_BOOLEAN,
        TYPE_STRING,
        TYPE_LIST,

        TYPE_FN,
    } tag;
    union {
        struct{
            TypeIdx from, to;
        } TYPE_FN;
        TypeIdx TYPE_LIST;
    } as;
} Type;

typedef struct {
    Type* data;
    usize len, allocated;
} TypeAllocator;

TypeAllocator TypeAllocator_new(void);
void TypeAllocator_free(TypeAllocator *alloc);

TypeIdx TypeAllocator_alloc(TypeAllocator *alloc, Type type);
void TypeAllocator_print(TypeAllocator *alloc, TypeIdx type);

#define TYPE_GET(alloc, idx) ((alloc)->data + idx)

#define TYPE_TAG(alloc, idx) (TYPE_GET((alloc), idx)->tag)
#define TYPE_VALUE(alloc, idx, tag) (TYPE_GET((alloc), idx)->as.tag)

#define TYPE_SET_V(alloc, idx, tag, value)                                     \
  *TYPE_GET(alloc, idx) = (Type) {                                             \
    tag, { .tag = value }                                                      \
  }
#define TYPE_SET_S(alloc, idx, tag, ...)                                       \
  *TYPE_GET(alloc, idx) = (Type) {                                             \
    tag, {                                                                     \
      .tag = { __VA_ARGS__ }                                                   \
    }                                                                          \
  }

#define TYPE_MAKE(alloc, tag) (TypeAllocator_alloc((alloc), (Type){tag, {0}}))
#define TYPE_MAKE_V(alloc, tag, value)                                         \
  (TypeAllocator_alloc((alloc), (Type){tag, {.tag = (value)}}))
#define TYPE_MAKE_S(alloc, tag, ...)                                           \
  (TypeAllocator_alloc((alloc), (Type){tag, {.tag = {__VA_ARGS__}}}))

#endif//WARLOCK_TYPES_H
