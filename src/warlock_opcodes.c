#include "warlock_opcodes.h"

#define TOSTRING(x) case(x): return STR(#x);
String Opcode_toString(Opcode op) {
    switch(op) {
        FOR_OPCODES(TOSTRING)
        default: return STR("OPCODE_INVALID");
    }
}
