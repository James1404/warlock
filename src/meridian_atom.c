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

void ListNode_free(ListNode* node) {
    if(node->next) {
        ListNode_free(node->next);
        free(node->next);
        node->next = NULL;
    }
}

void ListNode_push(ListNode* node, Atom atom) {
    ListNode* last = ListNode_last(node);

    ListNode* new = malloc(sizeof(ListNode));
    last->next = new;

    new->atom = atom;
}

ListNode* ListNode_last(ListNode* node) {
    while(node->next) {
        node = node->next;
    }

    return node;
}
