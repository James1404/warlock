#ifndef MERIDIAN_EVAL_H
#define MERIDIAN_EVAL_H

#include "meridian_atom.h"

void Eval_run(Atom atom);

bool Eval_match(Atom atom, const char* expected);

Atom Eval_Atom(Atom atom);
Atom Eval_List(Atom atom);

Atom Eval_Def(Atom atom);
Atom Eval_Fn(Atom atom);
Atom Eval_If(Atom atom);

#endif//MERIDIAN_EVAL_H
