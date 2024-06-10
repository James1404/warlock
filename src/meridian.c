#include "meridian.h"

#include "meridian_common.h"
#include "meridian_env.h"
#include "meridian_error.h"
#include "meridian_eval.h"
#include "meridian_printer.h"
#include "meridian_reader.h"

#include <stdio.h>
#include <stdlib.h>

void Meridian_init() {
    Env_init();
}

void Meridian_free() {
    Env_free();
}

#define BUILTIN(name, fn) Env_set(String_make(name), ATOM_INTRINSIC(&fn))
#define GLOBAL(name, value) Env_set(String_make(name), value)

Atom meridian_add(List args) {
    Atom result = ATOM_NUMBER(0);

    for(u64 i = 0; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_NUMBER(result) += GET_ATOM_NUMBER(args.data[i]);
    }

    return result;
}
Atom meridian_sub(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_NUMBER(result) -= GET_ATOM_NUMBER(args.data[i]);
    }

    return result;
}

Atom meridian_mul(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_NUMBER(result) *= GET_ATOM_NUMBER(args.data[i]);
    }

    return result;
}

Atom meridian_div(List args) {
    Atom result = args.data[0];

    for(u64 i = 1; i < args.length; i++) {
        // TODO: Add type checking for args (aka, make sure their all numbers)
        GET_ATOM_NUMBER(result) /= GET_ATOM_NUMBER(args.data[i]);
    }

    return result;
}

Atom meridian_head(List args) {
    if(args.length != 1) {
        Meridian_error("expected one arg");
        return ATOM_NIL();
    }

    Atom arg = args.data[0];

    if(arg.ty != ATOM_LIST) return arg;

    return GET_ATOM_LIST(arg).data[0];
}

Atom meridian_tail(List args) {
    if(args.length != 1) {
        Meridian_error("expected one arg");
        return ATOM_NIL();
    }

    Atom arg = args.data[0];

    if(arg.ty != ATOM_LIST) return arg;

    return GET_ATOM_LIST(arg).data[GET_ATOM_LIST(arg).length - 1];
}

Atom meridian_println(List args) {
    if(args.length == 0) {
        Meridian_error("expected one arg");
        return ATOM_NIL();
    }

    for(u64 i = 0; i < args.length; i++) {
        Printer_Atom(args.data[i]);
    }

    printf("\n");

    return ATOM_NIL();
}

void Meridian_builtin() {
    BUILTIN("+", meridian_add);
    BUILTIN("-", meridian_sub);
    BUILTIN("*", meridian_mul);
    BUILTIN("/", meridian_div);
    GLOBAL("true", ATOM_BOOLEAN(true));
    GLOBAL("false", ATOM_BOOLEAN(false));
    BUILTIN("println", meridian_println);

}

void Meridian_run(const char* src) {
    printf("--- INPUT ---\n");
    printf("%s\n", src);

    Reader reader = Reader_make(String_make(src));

    Reader_run(&reader);

    printf("--- Output ---\n");
    Eval_Atom(reader.global);

    Reader_free(&reader);
}

void Meridian_run_file(const char* path) {
    FILE* file = fopen(path, "r");

    if(!file) {
        Meridian_error("Couldnt not find file");
        return;
    }

    fseek(file, 0, SEEK_END);
    u64 length = ftell(file);

    fseek(file, 0, SEEK_SET);
    char* buffer = malloc(length);

    fread(buffer, 1, length, file);
    fclose(file);

    Meridian_run(buffer);

    free(buffer);
}
