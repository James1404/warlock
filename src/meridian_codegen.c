#include "meridian_codegen.h"

#include "meridian_common.h"
#include "meridian_error.h"
#include "meridian_printer.h"

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>

static LLVMContextRef context;
static LLVMBuilderRef builder;
static LLVMModuleRef module;

void codegen_atom(Atom atom) {
    switch(atom.type.type) {
        case TYPE_UNKNOWN: {
		} break;

        case TYPE_VAR:
        {
        } break;

        case TYPE_UNIT: {
		} break;

        case TYPE_BOOLEAN: {
		} break;

        case TYPE_INTEGER: {
		} break;
        case TYPE_REAL: {
		} break;

        case TYPE_STRING: {
		} break;
        case TYPE_FN: {
		} break;

        case TYPE_LIST: {
		} break;

        case TYPE_ANY: {
		} break;
    }
}

void codegen_add_int(List args) {
}

void codegen_sub_int(List args) {
}

void codegen_mul_int(List args) {
}

void codegen_div_int(List args) {
}

void codegen_add_real(List args) {
}

void codegen_sub_real(List args) {
}

void codegen_mul_real(List args) {
}

void codegen_div_real(List args) {
}

void codegen_eq(List args) {
}

void codegen_not(List args) {
}

void codegen_greater(List args) {
}

void codegen_lesser(List args) {
}

void codegen_and(List args) {
}

void codegen_or(List args) {
}

void codegen_head(List args) {
}

void codegen_tail(List args) {
}

void codegen_concat(List args) {
}

void codegen_println(List args) {
}

void Meridian_codegen(Atom root) {
    context = LLVMContextCreate();
    module = LLVMModuleCreateWithName("Meridian");
    builder = LLVMCreateBuilderInContext(context);

    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
    LLVMContextDispose(context);
}
