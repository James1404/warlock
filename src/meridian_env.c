#include "meridian_env.h"
#include "meridian_error.h"

#include <stdlib.h>

static Env env;

void Env_init() {
    env = (Env) {
        .scope = 0,

        .length = 0,
        .allocated = 0,
        .locals = NULL,
    };
}

void Env_free() {
    if(env.locals) free(env.locals);
    Env_init();
}

void Env_inc() {
    env.scope++;
}

void Env_dec() {
    env.scope--;

    while(env.locals[env.length - 1].scope > env.scope) {
        env.length--;
    }
}

Atom Env_get(String name) {
    for(u64 i = env.length - 1; i > 0; i--) {
        if(String_cmp(env.locals[i].name, name))
            return env.locals[i].atom;
    }

    Meridian_error("Could not find symbol in current environment");
    return ATOM_NIL();
}

void Env_set(String name, Atom atom) {
    if(!env.locals) {
        env.allocated = 8;
        env.locals = malloc(sizeof(Atom) * env.allocated);
    }

    if(env.length >= env.allocated) {
        env.allocated *= 2;

        Local* temp = realloc(env.locals, sizeof(Atom)*env.allocated);
        if(temp) {
            env.locals = temp;
        }
        else {
            Meridian_error("Env_set, realloc error");
        }
    }

    env.locals[env.length++] = (Local) {
        .scope = env.scope,
        .name = name,
        .atom = atom,
    };
}
