#ifndef WARLOCK_BUILTINS_H
#define WARLOCK_BUILTINS_H

#include "warlock_atom.h"

Sexp Sexp_Def(Environment *env, Sexp sexp);
Sexp Sexp_Fn(Environment *env, Sexp sexp);
Sexp Sexp_Quote(Environment* env, Sexp sexp);

Sexp Sexp_PrintEnv(Environment* env, Sexp sexp);

Sexp Sexp_Add(Environment *env, Sexp sexp);
Sexp Sexp_Subtract(Environment *env, Sexp sexp);
Sexp Sexp_Multiply(Environment *env, Sexp sexp);
Sexp Sexp_Divide(Environment *env, Sexp sexp);

Sexp Sexp_Equal(Environment *env, Sexp sexp);
Sexp Sexp_Not(Environment *env, Sexp sexp);

Sexp Sexp_Greater(Environment *env, Sexp sexp);
Sexp Sexp_Less(Environment *env, Sexp sexp);

Sexp Sexp_And(Environment *env, Sexp sexp);
Sexp Sexp_Or(Environment *env, Sexp sexp);

Sexp Sexp_Println(Environment *env, Sexp sexp);

Sexp Sexp_First(Environment *env, Sexp sexp);
Sexp Sexp_Rest(Environment *env, Sexp sexp);
Sexp Sexp_Len(Environment *env, Sexp sexp);
Sexp Sexp_Concat(Environment *env, Sexp sexp);

Sexp Sexp_Eval(Environment *env, Sexp sexp);
Sexp Sexp_Import(Environment *env, Sexp sexp);

Sexp Sexp_If(Environment* env, Sexp sexp);

#endif//WARLOCK_BUILTINS_H
