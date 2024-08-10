#ifndef SCHEME_EVAL_H
#define SCHEME_EVAL_H

#include "scheme_atom.h"

void Eval_run(Atom atom);

bool Eval_match(Atom atom, const char* expected);

Atom Eval_Atom(Atom atom);
Atom Eval_List(Atom atom);

Atom Eval_Define(Atom atom);
Atom Eval_Lambda(Atom atom);
Atom Eval_If(Atom atom);
Atom Eval_Quote(Atom atom);

#endif//SCHEME_EVAL_H
