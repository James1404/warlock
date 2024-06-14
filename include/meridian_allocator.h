#ifndef MERIDIAN_ALLOCATOR_H
#define MERIDIAN_ALLOCATOR_H

#include "meridian_common.h"

typedef struct Chunk {
    u8* data;

    struct Chunk* next;
} Chunk;

typedef struct {
    Chunk *start, *current;

    usize position;

    usize chunksize;
} Allocator;

Allocator Allocator_make(u64 chunksize);
void Allocator_free(Allocator* allocator);

void* Allocator_malloc(Allocator* allocator, usize size);

#endif//MERIDIAN_ALLOCATOR_H
