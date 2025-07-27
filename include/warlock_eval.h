#ifndef WARLOCK_EVAL_H
#define WARLOCK_EVAL_H

#include "warlock_atom.h"

Sexp Eval_run(Environment* env, Sexp sexp);

Sexp Eval_Atom(Environment *env, Sexp sexp);
Sexp Eval_Cons(Environment *env, Sexp sexp);
Sexp Eval_List(Environment *env, Sexp sexp);

Sexp Eval_Fn(Environment *env, Sexp sexp);
Sexp Eval_Def(Environment *env, Sexp sexp);
Sexp Eval_Quote(Environment *env, Sexp sexp);

#endif//WARLOCK_EVAL_H
