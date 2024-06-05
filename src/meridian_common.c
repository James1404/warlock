#include "meridian_common.h"

#include <stdlib.h>
#include <stdio.h>

String String_make(const byte* raw) {
    String r = {};

    r.length = strlen(raw);
    r.data = malloc(r.length + 1);
    memcpy(r.data, raw, r.length);
    r.data[r.length + 1] = '\0';

    return r;
}

void String_free(String* str) { /* Might not implement */ }

byte String_index(String str, i32 pos) {
    return str.data[pos];
}

String String_substr(String other, i32 pos, i32 length) {
    String r = {};

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

DynamicArray DynamicArray_make(u64 element_size) {
    return (DynamicArray) {
        .data = NULL,
        .length = 0,
        .capacity = 0,
        .element_size = element_size
    };
}
void DynamicArray_free(DynamicArray* array) {
    if(array->data) free(array->data);
    array->data = NULL;
}

void DynamicArray_push(DynamicArray* array, void* elem) {
    if(array->capacity == 0 || !array->data) {
        array->capacity = 8;
        array->data = malloc(array->element_size*array->capacity);
    }

    memcpy(&array->data[array->length], elem, array->element_size);
    array->length++;

    if(array->length >= array->capacity) {
        array->capacity *= 2;
        array->data = realloc(array->data, array->element_size*array->capacity);
    }
}
