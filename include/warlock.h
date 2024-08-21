#ifndef WARLOCK_H
#define WARLOCK_H

#include "warlock_atom.h"
#include "warlock_string.h"

void Warlock_init(void);
void Warlock_free(void);

void Warlock_builtin(SexpAllocator* alloc);
void Warlock_run(SexpAllocator* alloc, String src);
void Warlock_run_file(SexpAllocator* alloc, String path);

#endif//WARLOCK_H
