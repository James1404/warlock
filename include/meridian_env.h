#ifndef MERIDIAN_ENV_H
#define MERIDIAN_ENV_H

#include "meridian_atom.h"
#include "meridian_string.h"
#include "meridian_common.h"

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

void Env_init(void);
void Env_free(void);

void Env_inc(void);
void Env_dec(void);

Atom Env_get(String name);
void Env_set(String name, Atom atom);

#endif//MERIDIAN_ENV_H
