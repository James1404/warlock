#ifndef MERIDIAN_STRING_H
#define MERIDIAN_STRING_H

#include "meridian_common.h"

#define STR(s) String_make(s)

typedef struct {
    byte* data;
    i32 len;
} String;

String String_make(const byte* raw);
String String_copy(String other);

byte String_index(String str, i32 pos);
String String_substr(String other, i32 pos, i32 len);

const byte* String_get_raw(String str);

bool String_is(String str, const char* cmp);
bool String_cmp(String str, String other);

#endif//MERIDIAN_STRING_H
