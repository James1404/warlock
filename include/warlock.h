#ifndef WARLOCK_H
#define WARLOCK_H

#include "warlock_atom.h"
#include "warlock_string.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 01

void Warlock_builtin(Environment* env);
Sexp Warlock_run(Environment* env, String src);
Sexp Warlock_run_file(Environment* env, String path);

#endif//WARLOCK_H
