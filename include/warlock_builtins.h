#ifndef WARLOCK_BUILTINS_H
#define WARLOCK_BUILTINS_H

#include "warlock_atom.h"

Sexp Sexp_Add(Environment *alloc, Sexp sexp);
Sexp Sexp_Subtract(Environment *alloc, Sexp sexp);
Sexp Sexp_Multiply(Environment *alloc, Sexp sexp);
Sexp Sexp_Divide(Environment *alloc, Sexp sexp);

Sexp Sexp_Equal(Environment *alloc, Sexp sexp);
Sexp Sexp_Not(Environment *alloc, Sexp sexp);

Sexp Sexp_Greater(Environment *alloc, Sexp sexp);
Sexp Sexp_Less(Environment *alloc, Sexp sexp);

Sexp Sexp_And(Environment *alloc, Sexp sexp);
Sexp Sexp_Or(Environment *alloc, Sexp sexp);

Sexp Sexp_Println(Environment *alloc, Sexp sexp);

Sexp Sexp_First(Environment *alloc, Sexp sexp);
Sexp Sexp_Rest(Environment *alloc, Sexp sexp);
Sexp Sexp_Len(Environment *alloc, Sexp sexp);
Sexp Sexp_Concat(Environment *alloc, Sexp sexp);

Sexp Sexp_Eval(Environment *alloc, Sexp sexp);
Sexp Sexp_Import(Environment *alloc, Sexp sexp);

Sexp Sexp_If(Environment *alloc, Sexp sexp);

#endif//WARLOCK_BUILTINS_H
