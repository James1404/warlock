#ifndef WARLOCK_EVAL_H
#define WARLOCK_EVAL_H

#include "warlock_atom.h"

Sexp Eval_run(SexpAllocator* alloc, Sexp sexp);

Sexp Eval_TopLevel(SexpAllocator* alloc, Sexp sexp);
Sexp Eval_Atom(SexpAllocator *alloc, Sexp sexp);
Sexp Eval_Cons(SexpAllocator *alloc, Sexp sexp);

Sexp Eval_Fn(SexpAllocator *alloc, Sexp sexp);
Sexp Eval_Def(SexpAllocator *alloc, Sexp sexp);
Sexp Eval_Quote(SexpAllocator *alloc, Sexp sexp);

#endif//WARLOCK_EVAL_H
