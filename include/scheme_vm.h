#ifndef SCHEME_VM_H
#define SCHEME_VM_H

#include "scheme_atom.h"
#include "scheme_opcodes.h"

typedef struct {
    Opcode *code;
    u64 length, allocated;
} Chunk;

typedef struct {
    usize ip;

    Atom* stack;
    usize stack_allocated;
    usize stack_top;
} VM;

VM VM_make(void);
void VM_free(VM* vm);

void VM_run(VM* vm, Chunk* code);

void VM_push(VM* vm, Atom atom);
Atom VM_pop(VM* vm);

#endif//SCHEME_VM_H
