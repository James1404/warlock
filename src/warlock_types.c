#include "warlock_types.h"

#include <stdlib.h>
#include <stdio.h>


TypeAllocator TypeAllocator_new(void) {
    return (TypeAllocator) {
        .data = NULL,
        .len = 0,
        .allocated = 0,
    };
}
void TypeAllocator_free(TypeAllocator *alloc) {
    free(alloc->data);
}

TypeIdx TypeAllocator_alloc(TypeAllocator *alloc, Type type) {
    if(!alloc->data) {
        alloc->allocated = 8;
        alloc->data = malloc(sizeof(*alloc->data) * alloc->allocated);
    }

    if(alloc->len >= alloc->allocated) {
        alloc->allocated *= 2;
        alloc->data = realloc(alloc->data, sizeof(*alloc->data) * alloc->allocated);
    }

    TypeIdx idx = alloc->len;
    alloc->data[idx] = type;
    alloc->len++;
    
    return idx;
}

void TypeAllocator_print(TypeAllocator* alloc, TypeIdx type) {
}

