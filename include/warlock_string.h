#ifndef WARLOCK_STRING_H
#define WARLOCK_STRING_H

#include "warlock_common.h"

#define STR(x) ((String){.raw = x, .len = strlen(x)})
#define STR_ALLOC(l) ((String){.raw = malloc(l), .len = l});
#define STR_CPY_ALLOC(dst, src)                                                \
  do {                                                                         \
    dst = STR_ALLOC((src).len);                                                \
    strncpy((dst).raw, (src).raw, (src).len);                                  \
  } while (0)
#define STR_CPY_ALLOC_NULL(dst, src)                                                \
  do {                                                                         \
    dst = STR_ALLOC((src).len + 1);                                                \
    strncpy((dst).raw, (src).raw, (src).len);                                  \
    dst.raw[(src).len] = '\0';\
  } while (0)

#define STR_FREE(str)                                                          \
  do {                                                                         \
    free(str.raw);                                                             \
  } while (0)

typedef struct {
    char* raw;
    i32 len;
} String;

#define STR_CMP(lhs, rhs) (strncmp((lhs).raw, (rhs).raw, (lhs).len) == 0)
#define STR_CMP_WITH_RAW(lhs, rhs) (strncmp((lhs).raw, (rhs), strlen(rhs)) == 0)
#define STR_CMP_RAWS(lhs, rhs) (strcmp((lhs), (rhs)) == 0)

#define STR_IDX(src, i) ((src).raw[i])
#define STR_SUBSTR(src, start, len) ((String){(src).raw + start, len})

#endif//WARLOCK_STRING_H
