#include "meridian_common.h"

#include <stdlib.h>
#include <stdio.h>

#define STR(s) String_make(s)

String String_make(const byte* raw) {
    String r;

    r.length = strlen(raw);
    r.data = malloc(r.length + 1);
    memcpy(r.data, raw, r.length);
    r.data[r.length] = '\0';

    return r;
}

String String_copy(String other) {
    String r;

    r.length = other.length;
    r.data = malloc(r.length + 1);
    memcpy(r.data, other.data, r.length);
    r.data[r.length] = '\0';

    return r;
}

byte String_index(String str, i32 pos) {
    return str.data[pos];
}

String String_substr(String other, i32 pos, i32 length) {
    String r;

    r.data = malloc(length + 1);
    memcpy(r.data, other.data + pos, sizeof(byte) * length);
    r.data[length] = '\0';
    r.length = length;
    return r;
}

String String_concat_with_raw(String lhs, const char* rhs) {
    u64 rhs_len = strlen(rhs);
    u64 size = lhs.length + rhs_len;

    String r;

    r.data = malloc(size + 1);
    r.length = size;
    memcpy(r.data, lhs.data, lhs.length);
    memcpy(r.data + rhs_len, rhs, rhs_len);
    r.data[size] = '\0';

    return r;
}

String String_concat(String lhs, String rhs) {
    u64 size = lhs.length + rhs.length;

    String r;

    r.data = malloc(size + 1);
    r.length = size;
    memcpy(r.data, lhs.data, lhs.length);
    memcpy(r.data + rhs.length, rhs.data, rhs.length);
    r.data[size] = '\0';

    return r;
}

const byte* String_get_raw(String str) {
    byte* buffer = malloc(str.length + 1);

    snprintf(buffer, str.length + 1, "%.*s", (int)str.length, str.data);

    return buffer;
}

bool String_is(String str, const char* cmp) {
    return strncmp(str.data, cmp, str.length) == 0;
}

bool String_cmp(String str, String other) {
    return strncmp(str.data, other.data, str.length) == 0;
}
