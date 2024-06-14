#include "meridian_allocator.h"

#include <stdlib.h>

static void Chunk_free(Chunk* chunk) {
    if(chunk->data) {
        free(chunk->data);
    }

    if(chunk->next) {
        Chunk_free(chunk->next);
        free(chunk->next);
    }
}

Allocator Allocator_make(u64 chunksize) {
    return (Allocator) {
        .start = NULL,
        .current = NULL,
        .position = 0,
        .chunksize = chunksize,
    };
}

void Allocator_free(Allocator* allocator) {
    if(allocator->start) {
        Chunk_free(allocator->start);
    }

    *allocator = Allocator_make(allocator->chunksize);
}

void* Allocator_malloc(Allocator* allocator, usize size) {
    if(!allocator->start) {
        allocator->start = malloc(sizeof(Chunk));
        allocator->current = allocator->start;
        allocator->position = 0;
    }

    void* start = allocator->current->data + allocator->position;

    return start;
}
