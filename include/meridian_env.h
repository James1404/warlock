#ifndef MERIDIAN_ENV_H
#define MERIDIAN_ENV_H

#include "meridian_atom.h"

typedef struct {
    u64 scope;
    String name;
    Atom atom;
} Local;

typedef struct {
    u64 scope;

    u64 length, allocated;
    Local* locals;
} Env;

void Env_init();
void Env_free();

void Env_inc();
void Env_dec();

Atom Env_get(String name);
void Env_set(String name, Atom atom);

#endif//MERIDIAN_ENV_H
