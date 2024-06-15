#include "meridian_allocator.h"

#include <stdlib.h>

static Chunk* Chunk_make(Allocator* parent) {
    Chunk* c = malloc(sizeof(Chunk));
    *c = (Chunk) {
        .data = malloc(parent->chunksize),
        .used = 0,
        .next = NULL
    };

    return c;
}

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
        allocator->start = Chunk_make(allocator);
        allocator->current = allocator->start;
    }

    if(size <= allocator->chunksize - allocator->current->used) {
        allocator->current->next = Chunk_make(allocator);
    }

    void* position = allocator->current->data + allocator->current->used;
    allocator->current->used += size;

    return position;
}

static Allocator main;

void MainAllocator_init() {
    main = Allocator_make(sizeof(u64) * 100000);
}

void MainAllocator_free() {
    Allocator_free(&main);
}

void* MainAllocator_malloc(usize size) {
    return Allocator_malloc(&main, size);
}
