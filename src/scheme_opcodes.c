#include "scheme_opcodes.h"

#define TOSTRING(x) case(x): return String_make(#x);
String Opcode_toString(Opcode op) {
    switch(op) {
        FOR_OPCODES(TOSTRING)
        default: return String_make("OPCODE_INVALID");
    }
}
