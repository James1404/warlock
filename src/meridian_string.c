#include "meridian_string.h"

#include "meridian_allocator.h"

#include <stdio.h>
#include <stdlib.h>

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
