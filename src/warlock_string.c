#include "warlock_string.h"

#include <stdlib.h>
#include <string.h>

inline String String_from(const char* str) {
    u32 len = strlen(str);

    char* ptr = malloc(len);
    memcpy(ptr, str, len);

    return (String){
        .raw = ptr,
        .len = len,
    };
}

inline void String_free(String* str) {
    free(str->raw);
    str->raw = NULL;
}

inline bool String_cmp(String lhs, String rhs) {
    return strncmp(lhs.raw, rhs.raw, lhs.len) == 0;
}

inline bool String_cmp_with_raw(String lhs, const char* rhs) {
    return strncmp(lhs.raw, rhs, lhs.len) == 0;
}

inline char String_idx(String str, i32 idx) { return str.raw[idx]; }

inline String String_substr(String str, i32 start, i32 len) {
    return (String){
        .raw = str.raw + start,
        .len = len,
    };
}

inline String String_copy(String str) {
    char* ptr = malloc(str.len);
    memcpy(ptr, str.raw, str.len);

    return (String){
        .raw = ptr,
        .len = str.len,
    };
}

inline String String_copy_null(String str) {
    char* ptr = malloc(str.len + 1);
    memcpy(ptr, str.raw, str.len);
    ptr[str.len] = '\0';

    return (String){
        .raw = ptr,
        .len = str.len,
    };
}
