#include "scheme_string.h"

#include <stdio.h>
#include <stdlib.h>

String String_make(const byte* raw) {
    String r;

    r.len = strlen(raw);
    r.data = malloc(r.len + 1);
    memcpy(r.data, raw, r.len);
    r.data[r.len] = '\0';

    return r;
}

String String_copy(String other) {
    String r;

    r.len = other.len;
    r.data = malloc(r.len + 1);
    memcpy(r.data, other.data, r.len);
    r.data[r.len] = '\0';

    return r;
}

byte String_index(String str, i32 pos) {
    return str.data[pos];
}

String String_substr(String other, i32 pos, i32 len) {
    String r;

    r.data = malloc(len + 1);
    memcpy(r.data, other.data + pos, sizeof(byte) * len);
    r.data[len] = '\0';
    r.len = len;
    return r;
}

const byte* String_get_raw(String str) {
    byte* buffer = malloc(str.len + 1);

    snprintf(buffer, str.len + 1, "%.*s", (int)str.len, str.data);

    return buffer;
}

bool String_is(String str, const char* cmp) {
    return strncmp(str.data, cmp, str.len) == 0;
}

bool String_cmp(String str, String other) {
    return strncmp(str.data, other.data, str.len) == 0;
}
