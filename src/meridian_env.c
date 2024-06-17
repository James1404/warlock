#include "meridian_env.h"
#include "meridian_error.h"

#include <stdlib.h>
#include <stdio.h>

static Env env;

void Env_init(void) {
    env = (Env) {
        .scope = 0,

        .length = 0,
        .allocated = 0,
        .locals = NULL,
    };
}

void Env_free(void) {
    if(env.locals) free(env.locals);
    Env_init();
}

void Env_inc(void) {
    env.scope++;
}

void Env_dec(void) {
    env.scope--;

    while(env.locals[env.length - 1].scope > env.scope) {
        env.length--;
    }
}

Atom Env_get(String name) {
    for(i64 i = env.length - 1; i >= 0; i--) {
        Local entry = env.locals[i];

        if(String_cmp(entry.name, name)) {
            return entry.atom;
        }
    }

    Meridian_error("Could not find symbol in current environment");
    return ATOM_NIL();
}

void Env_set(String name, Atom atom) {
    if(!env.locals) {
        env.length = 0;
        env.allocated = 8;
        env.locals = malloc(sizeof(Local) * env.allocated);
    }

    if(env.length >= env.allocated) {
        env.allocated *= 2;

        Local* temp = realloc(env.locals, sizeof(Local) * env.allocated);
        if(temp) {
            env.locals = temp;
        }
        else {
            Meridian_error("Env_set, realloc error");
            return;
        }
    }

    env.locals[env.length++] = (Local) {
        .scope = env.scope,
        .name = String_copy(name),
        .atom = atom,
    };
}
