#include "scheme_vm.h"

#include "scheme_error.h"

#include <stdlib.h>

VM VM_make(void) {
    return (VM) {
        .ip = 0,
    };
}

void VM_free(VM* vm) {
}

void VM_run(VM* vm, Chunk* code) {
}

void VM_push(VM* vm, Atom atom) {
    if(!vm->stack) {
        vm->stack_allocated = 8;
        vm->stack_top = 0;
        vm->stack = malloc(sizeof(Atom) * vm->stack_allocated);
    }

    if(vm->stack_top >= vm->stack_allocated) {
        vm->stack_allocated *= 2;

        Atom* temp = realloc(vm->stack, sizeof(Atom) * vm->stack_allocated);
        if(temp) {
            vm->stack = temp;
        }
        else {
            Scheme_error("VM_push realloc error");
            return;
        }
    }

    vm->stack[vm->stack_top++] = atom;
}

Atom VM_pop(VM* vm) {
    return vm->stack[vm->stack_top];
}
