#ifndef MERIDIAN_OPCODES_H
#define MERIDIAN_OPCODES_H

#include "meridian_string.h"

#define FOR_OPCODES(DO)\
    DO(ADD)\
    DO(SUB)\
    DO(MUL)\
    DO(DIV)\
\
    DO(EQ) DO(NEQ) DO(GREATER) DO(LESS) DO(GREATER_EQ) DO(LESS_EQ)\
    DO(PUSH) DO(POP)\
    DO(CONSTANT)\
\
    DO(JUMP)\

#define OPCODE(x) OPCODE_ ## x

typedef enum {
#define DEF_ENUM(x) OPCODE(x),
    FOR_OPCODES(DEF_ENUM)
#undef DEF_ENUM
} Opcode;

String Opcode_toString(Opcode op);

#endif//MERIDIAN_OPCODES_H
