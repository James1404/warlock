#include "warlock_allocator.h"

#include "warlock_arena.h"
#include "warlock_atom.h"

static Arena arena;

void Warlock_alloc_init(void) { arena = Arena_make(); }
void Warlock_alloc_deinit(void) { Arena_free(&arena); }

Sexp Warlock_atom(Atom atom) {
    Sexp sexp = Arena_malloc(&arena, sizeof(Atom));
    *sexp = atom;
    return sexp;
}

void* Warlock_malloc(usize size) { return Arena_malloc(&arena, size); }
