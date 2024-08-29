#ifndef WARLOCK_BUILTINS_H
#define WARLOCK_BUILTINS_H

#include "warlock_atom.h"

Sexp Sexp_Add(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Subtract(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Multiply(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Divide(SexpAllocator *alloc, Sexp sexp);

Sexp Sexp_Equal(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Not(SexpAllocator *alloc, Sexp sexp);

Sexp Sexp_Greater(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Less(SexpAllocator *alloc, Sexp sexp);

Sexp Sexp_And(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Or(SexpAllocator *alloc, Sexp sexp);

Sexp Sexp_Println(SexpAllocator *alloc, Sexp sexp);

Sexp Sexp_First(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Rest(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Len(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Concat(SexpAllocator *alloc, Sexp sexp);

Sexp Sexp_Eval(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Import(SexpAllocator *alloc, Sexp sexp);

Sexp Sexp_Quote(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Define(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_If(SexpAllocator *alloc, Sexp sexp);
Sexp Sexp_Lambda(SexpAllocator *alloc, Sexp sexp);

#endif//WARLOCK_BUILTINS_H
