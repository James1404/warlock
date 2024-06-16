#include "meridian_vm.h"

VM VM_make() {
    return (VM) {
        .ip = 0,
    };
}

void VM_free(VM* vm) {
}

void VM_run(VM* vm, Chunk* code) {
}
