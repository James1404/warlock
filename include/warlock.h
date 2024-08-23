#ifndef WARLOCK_H
#define WARLOCK_H

#include "warlock_atom.h"
#include "warlock_string.h"

void Warlock_builtin(SexpAllocator* alloc);
Sexp Warlock_run(SexpAllocator* alloc, String src);
Sexp Warlock_run_file(SexpAllocator* alloc, String path);

#endif//WARLOCK_H
