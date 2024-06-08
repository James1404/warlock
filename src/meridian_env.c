#include "meridian_env.h"
#include "meridian_error.h"

#include <stdlib.h>

Env Env_make() {
    return (Env) {
        .scope = 0,

        .length = 0,
        .allocated = 0,
        .locals = NULL,
    };
}

void Env_free(Env* env) {
    if(env->locals) free(env->locals);
    *env = Env_make();
}

void Env_inc(Env* env) {
    env->scope++;
}

void Env_dec(Env* env) {
    env->scope--;

    while(env->locals[env->length - 1].scope > env->scope) {
        env->length--;
    }
}

Atom Env_get(Env* env, String name) {
    u64 i = env->length;
    while(!String_cmp(env->locals[i].name, name)) {
        if(i < 0) {
            Meridian_error("Could not find symbol in current environment");
            return ATOM_NIL();
        }
    }

    return env->locals[i].value;
}

void Env_set(Env* env, String name, Atom atom) {
    if(!env->locals) {
        env->allocated = 8;
        env->locals = malloc(sizeof(Atom) * env->allocated);
    }

    env->locals[env->length] = (Local) {
        .scope = env->scope,
        .name = name,
        .value = atom,
    };

    env->length++;

    if(env->length >= env->allocated) {
        env->allocated *= 2;
        env->locals = realloc(env->locals, sizeof(Atom)*env->allocated);
    }
}
