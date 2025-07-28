#include "warlock_common.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* format_string(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    usize len = vsnprintf(NULL, 0, fmt, args);
    char* buf = malloc(len + 1);
    vsnprintf(buf, len + 1, fmt, args);

    va_end(args);

    return buf;
}

char* copy_string(char* str) {
    usize len = strlen(str);
    char* buf = malloc(len + 1);
    strcpy(buf, str);
    buf[len] = '\0';
    return buf;
}

bool string_equal(char* lhs, char* rhs) {
    return strcmp(lhs, rhs) == 0;
}

char* string_substr(char* lhs, usize start, usize len) {
    char* buf = malloc(len + 1);

    strncpy(buf, lhs + start, len);
    buf[len] = '\0';

    return buf;
}
