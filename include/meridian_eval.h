#ifndef MERIDIAN_EVAL_H
#define MERIDIAN_EVAL_H

#include "meridian_atom.h"
#include "meridian_env.h"
#include "meridian_intrinsics.h"

typedef struct {
    Env env;
    IntrinsicMap intrinsics;
} Eval;

Eval Eval_make();
void Eval_free(Eval* eval);

void Eval_run(Eval* eval, Atom atom);

bool Eval_match(Eval* eval, Atom atom, const char* expected);

Atom Eval_Atom(Eval* eval, Atom atom);
Atom Eval_List(Eval* eval, Atom atom);

Atom Eval_Def(Eval* eval, Atom atom);
Atom Eval_Fn(Eval* eval, Atom atom);

#endif//MERIDIAN_EVAL_H
