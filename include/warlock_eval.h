#ifndef WARLOCK_EVAL_H
#define WARLOCK_EVAL_H

#include "warlock_atom.h"

Sexp Eval_run(Environment* alloc, Sexp sexp);

Sexp Eval_Atom(Environment *alloc, Sexp sexp);
Sexp Eval_Cons(Environment *alloc, Sexp sexp);
Sexp Eval_List(Environment *alloc, Sexp sexp);

Sexp Eval_Fn(Environment *alloc, Sexp sexp);
Sexp Eval_Def(Environment *alloc, Sexp sexp);
Sexp Eval_Quote(Environment *alloc, Sexp sexp);

#endif//WARLOCK_EVAL_H
