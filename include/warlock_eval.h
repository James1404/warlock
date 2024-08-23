#ifndef WARLOCK_EVAL_H
#define WARLOCK_EVAL_H

#include "warlock_atom.h"

Sexp Eval_run(SexpAllocator* alloc, Sexp sexp);

bool Eval_match(SexpAllocator* alloc, Sexp sexp, const char* expected);

Sexp Eval_Atom(SexpAllocator* alloc, Sexp sexp);
Sexp Eval_Cons(SexpAllocator* alloc, Sexp sexp);

Sexp Eval_Define(SexpAllocator* alloc, Sexp sexp);
Sexp Eval_Lambda(SexpAllocator* alloc, Sexp sexp);
Sexp Eval_If(SexpAllocator* alloc, Sexp sexp);
Sexp Eval_Quote(SexpAllocator* alloc, Sexp sexp);

#endif//WARLOCK_EVAL_H
