#include "warlock_arena.h"

#include <stdlib.h>
#include <string.h>

#define ARENA_PAGE_LEN 4096

static ArenaPage* ArenaPage_make(void) {
    ArenaPage* ptr = malloc(sizeof(ArenaPage));

    *ptr = (ArenaPage){
        .data = malloc(ARENA_PAGE_LEN),
        .used = 0,
        .allocated = ARENA_PAGE_LEN,

        .next = NULL,
    };

    return ptr;
}

static ArenaPage* ArenaPage_make_extended(usize size) {
    ArenaPage* ptr = malloc(sizeof(ArenaPage));

    *ptr = (ArenaPage){
        .data = malloc(size),
        .used = 0,
        .allocated = size,

        .next = NULL,
    };

    return ptr;
}

static void ArenaPage_free(ArenaPage* page) {
    if (page->next) {
        ArenaPage_free(page->next);
    }

    free(page->data);
    free(page);
}

Arena Arena_make(void) {
    ArenaPage* head = ArenaPage_make();

    return (Arena){
        .head = head,
        .tail = head,
    };
}

void Arena_free(Arena* arena) { ArenaPage_free(arena->head); }

static void Arena_new_page(Arena* arena, usize size) {
    ArenaPage* page = arena->tail;
    ArenaPage* newpage = ArenaPage_make_extended(size);

    page->next = newpage;
    arena->tail = newpage;
    page = newpage;
}

void* Arena_malloc(Arena* arena, usize size) {
    ArenaPage* page = arena->tail;

    if (size > ARENA_PAGE_LEN) {
        ArenaPage* newpage = ArenaPage_make_extended(size);

        page->next = newpage;
        arena->tail = newpage;
        page = newpage;

        void* ptr = page->data;

        Arena_new_page(arena, ARENA_PAGE_LEN);

        return ptr;
    }
    else {
        if (page->used + size >= page->allocated) {
            ArenaPage* newpage = ArenaPage_make();

            page->next = newpage;
            arena->tail = newpage;
            page = newpage;
        }

        void* ptr = (void*)(page->data + page->used);
        page->used += size;

        return ptr;
    }
}
