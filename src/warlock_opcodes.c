#include "warlock_opcodes.h"

#define TOSTRING(x) case(x): return #x;
char* Opcode_toString(Opcode op) {
    switch(op) {
        FOR_OPCODES(TOSTRING)
        default: return "OPCODE_INVALID";
    }
}
