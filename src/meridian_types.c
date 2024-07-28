#include "meridian_types.h"
#include "meridian_error.h"

#include <stdlib.h>

void Type_add_arg(Type* type, Type arg) {
    u64 idx = type->extra.fn.argc++;

    type->extra.fn.args = realloc(type->extra.fn.args, sizeof(type->extra.fn.args[0]) * type->extra.fn.argc);

    type->extra.fn.args[idx] = arg;
}

static TypeEnv env;

void TypeEnv_init(void) {
    env = (TypeEnv) {
        .scope = 0,

        .length = 0,
        .allocated = 0,
        .locals = NULL,
    };
}

void TypeEnv_free(void) {
    free(env.locals);
    TypeEnv_init();
}

void TypeEnv_inc(void) {
    env.scope++;
}

void TypeEnv_dec(void) {
    env.scope--;

    while(env.locals[env.length - 1].scope > env.scope) {
        env.length--;
    }
}

Type TypeEnv_get(String name) {
    for(i64 i = env.length - 1; i >= 0; i--) {
        TypeLocal entry = env.locals[i];

        if(String_cmp(entry.name, name)) {
            return entry.type;
        }
    }

    Meridian_error("Could not find symbol in current environment");
    return TYPE_ANY();
}

void TypeEnv_set(String name, Type type) {
    if(!env.locals) {
        env.length = 0;
        env.allocated = 8;
        env.locals = malloc(sizeof(TypeLocal) * env.allocated);
    }

    if(env.length >= env.allocated) {
        env.allocated *= 2;

        TypeLocal* temp = realloc(env.locals, sizeof(TypeLocal) * env.allocated);
        if(temp) {
            env.locals = temp;
        }
        else {
            Meridian_error("TypeEnv_set realloc error");
            return;
        }
    }

    env.locals[env.length++] = (TypeLocal) {
        .scope = env.scope,
        .name = String_copy(name),
        .type = type,
    };
}
