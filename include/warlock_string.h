#ifndef WARLOCK_STRING_H
#define WARLOCK_STRING_H

#include "warlock_common.h"

#include <stdlib.h>
#include <string.h>

#define STR(x) ((String){.raw = x, .len = strlen(x)})
#define STR_ALLOC(l) ((String){.raw = malloc(l), .len = l});

typedef struct String {
    char* raw;
    i32 len;
} String;

String String_from(const char* str);
void String_free(String* str);

bool String_cmp(String lhs, String rhs);
bool String_cmp_with_raw(String lhs, const char* rhs);

char String_idx(String str, i32 idx);
String String_substr(String str, i32 start, i32 len);

String String_copy(String str);
String String_copy_null(String str);

typedef struct StringView {
    char* raw;
    i32 len;
} StringView;

StringView StringView_make(const char* str);

#endif // WARLOCK_STRING_H
