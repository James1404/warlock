#ifndef MERIDIAN_ENV_H
#define MERIDIAN_ENV_H

#include "meridian_atom.h"

typedef struct {
    u64 scope;
    String name;
    Atom value;
} Local;

typedef struct {
    u64 scope;

    u64 length, allocated;
    Local* locals;
} Env;

Env Env_make();
void Env_free(Env* env);

void Env_inc(Env* env);
void Env_dec(Env* env);

Atom Env_get(Env* env, String name);
void Env_set(Env* env, String name, Atom atom);

#endif//MERIDIAN_ENV_H
