#include "meridian_common.h"

#include <stdlib.h>
#include <stdio.h>

String String_make(const byte* raw) {
    String r = {};

    r.length = strlen(raw);
    r.data = malloc(r.length + 1);
    memcpy(r.data, raw, r.length);
    r.data[r.length] = '\0';

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

String String_concat_with_raw(String lhs, const char* rhs) {
    u64 rhs_len = strlen(rhs);
    u64 size = lhs.length + rhs_len;

    String r = {};

    r.data = malloc(size + 1);
    r.length = size;
    memcpy(r.data, lhs.data, lhs.length);
    memcpy(r.data + rhs_len, rhs, rhs_len);
    r.data[size] = '\0';

    return r;
}

String String_concat(String lhs, String rhs) {
    u64 size = lhs.length + rhs.length;

    String r = {};

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

ArenaAllocator ArenaAllocator_make(u64 region_size) {
    return (ArenaAllocator) {
        .size = region_size,
        .length = 0,
        .regions = NULL
    };
}

void ArenaAllocator_free(ArenaAllocator* allocator) {
    for(u32 i = 0; i < allocator->length; i++) {
        Region region = allocator->regions[i];
        free(region.data);
    }

    if(allocator->regions) free(allocator->regions);
}

void* ArenaAllocator_alloc(ArenaAllocator* allocator, u64 size) {
    if(size > allocator->size) return NULL;

    u64 idx = allocator->length % allocator->size;
    u64 region = allocator->length / allocator->size;

    allocator->length++;

    if(!allocator->regions || idx > allocator->length) {
        allocator->length++;
        allocator->regions = malloc(allocator->size * allocator->length);
    }

    allocator->regions[region].used++;
    return &allocator->regions[region].data[idx];
}
