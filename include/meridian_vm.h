#ifndef MERIDIAN_VM_H
#define MERIDIAN_VM_H

#include "meridian_opcodes.h"
typedef struct {
    Opcode *code;
    u64 length, allocated;
} Chunk;

typedef struct {
    usize ip;
} VM;

VM VM_make();
void VM_free(VM* vm);

void VM_run(VM* vm, Chunk* code);

#endif//MERIDIAN_VM_H
