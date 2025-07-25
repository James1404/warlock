#ifndef WARLOCK_ARENA_H
#define WARLOCK_ARENA_H

#include "warlock_common.h"

#define ARENA_PAGE_LEN 4096

typedef struct ArenaPage {
    u8* data;
    u64 used;

    struct ArenaPage* next;
} ArenaPage;

typedef struct Arena {
    ArenaPage *head, *tail;
} Arena;

Arena Arena_make(void);
void Arena_free(Arena* arena);

void* Arena_malloc(Arena* arena, usize size);

#endif // WARLOCK_ARENA_H
