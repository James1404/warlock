#ifndef WARLOCK_ALLOCATOR_H
#define WARLOCK_ALLOCATOR_H

#include "warlock_atom.h"

void Warlock_alloc_init(void);
void Warlock_alloc_deinit(void);

Sexp Warlock_atom(Atom atom);
void* Warlock_malloc(usize size);

#endif // WARLOCK_ALLOCATOR_H
