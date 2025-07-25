#include "warlock_arena.h"

#include <stdlib.h>

static ArenaPage* ArenaPage_make(void) {
    ArenaPage* ptr = malloc(sizeof(ArenaPage));

    *ptr = (ArenaPage) {
        .data = malloc(ARENA_PAGE_LEN),
        .used = 0,

        .next = NULL,
    };

    return ptr;
}

static void ArenaPage_free(ArenaPage* page) {
    if(page->next) {
        ArenaPage_free(page->next);
    }
    
    free(page->data);
    free(page);
}

Arena Arena_make(void) {
    ArenaPage* head = ArenaPage_make();

    return (Arena) {
        .head = head,
        .tail = head,
    };
}

void Arena_free(Arena* arena) {
    ArenaPage_free(arena->head);
}

void* Arena_malloc(Arena* arena, usize size) {
    ArenaPage* page = arena->tail;

    if(page->used + size >= ARENA_PAGE_LEN) {
        ArenaPage* newpage = ArenaPage_make();
        page->next = newpage;
        arena->tail = newpage;
        page = newpage;        
    }

    void* ptr = (void*)(page->data + page->used);
    page->used += size;
    
    return ptr;
}
