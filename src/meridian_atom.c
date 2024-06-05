#include "meridian_atom.h"

#include <stdlib.h>

#define DEFINE_ATOM_TOSTRING(x)\
    case ATOM_GET_TYPE(x): return String_make(#x); break;

String Atom_toString(Atom* atom) {
    switch(atom->type) {
        FOR_PRIMITIVES(DEFINE_ATOM_TOSTRING)
    }
}

FOR_PRIMITIVES(DEFINE_ATOM_MAKE_IMPL);
