#include "meridian_printer.h"

#include <stdio.h>

void Printer_Atom(Atom atom) {
    switch(atom.ty) {
        case ATOM_NUMBER: {
            printf("%Lf", GET_ATOM_NUMBER(atom));
        } break;
        case ATOM_BOOLEAN: {
            bool v = GET_ATOM_BOOLEAN(atom);
            if(v)
                printf("#t");
            else
                printf("#f");
        } break;
        case ATOM_STRING: {
            String str = GET_ATOM_STRING(atom);
            printf("\"%.*s\"", str.length, str.data);
        } break;
        case ATOM_SYMBOL: {
            String sym = GET_ATOM_SYMBOL(atom);
            printf("%.*s", sym.length, sym.data);
        } break;
        case ATOM_KEYWORD: {
            String key = GET_ATOM_KEYWORD(atom);
            printf("%.*s", key.length, key.data);
        } break;
        case ATOM_LIST: {
            List l = GET_ATOM_LIST(atom);

            printf("(");
            
            for(u32 i = 0; i < l.length; i++) {
                Atom atom = List_at(&l, i);
                Printer_Atom(atom);
                if(i < l.length - 1) printf(" ");
            }

            printf(")");
            break;
        } break;
        case ATOM_FN: {
            Fn func = GET_ATOM_FN(atom);
            
            printf("(");
            printf("fn ");
            for(u32 i = 0; i < func.args_length; i++) {
                printf("%s", String_get_raw(func.args[i]));
                if(i < func.args_length - 1) printf(" ");
            }
            printf(" -> ");
            Printer_Atom(*func.body);
            printf(")");

            break;
        } break;
        case ATOM_INTRINSIC: {
            printf("#intrinsic");
            break;
        } break;
        case ATOM_FFI: {
            printf("ffi");
            break;
        } break;
        case ATOM_NIL: {
            printf("NIL");
        } break;
    }
}

void Printer_run(Atom atom) {
    Printer_Atom(atom);
    printf("\n");
}
