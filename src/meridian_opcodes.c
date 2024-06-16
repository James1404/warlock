#include "meridian_opcodes.h"

#define TOSTRING(x) case(OPCODE(x)): return String_make(#x);
String Opcode_toString(Opcode op) {
    switch(op) {
        FOR_OPCODES(TOSTRING)
    }
}
