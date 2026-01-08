#ifndef WARLOCK_OPCODES_H
#define WARLOCK_OPCODES_H

#define FOR_OPCODES(DO)\
    DO(OP_ADD)\
    DO(OP_SUB)\
    DO(OP_MUL)\
    DO(OP_DIV)\
\
    DO(OP_EQ) DO(OP_NEQ) DO(OP_GREATER) DO(OP_LESS) DO(OP_GREATER_EQ) DO(OP_LESS_EQ)\
    DO(OP_PUSH) DO(OP_POP)\
    DO(OP_CONSTANT)\
\
    DO(OP_JUMP)\

typedef enum {
#define DEF_ENUM(x) x,
    FOR_OPCODES(DEF_ENUM)
#undef DEF_ENUM
} Opcode;

char* Opcode_toString(Opcode op);

#endif//WARLOCK_OPCODES_H
