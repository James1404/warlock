#ifndef MERIDIAN_INTRINSICS_H
#define MERIDIAN_INTRINSICS_H

#include "meridian_atom.h"

typedef Atom (*Intrinsic)();

typedef struct IntrinsicEntry {
    usize hash;
    String key;
    Intrinsic value;

    struct IntrinsicEntry* next;
} IntrinsicEntry;

typedef struct {
    usize allocated, length;
    IntrinsicEntry* buckets;
} IntrinsicMap;

IntrinsicMap IntrinsicMap_make();
void IntrinsicMap_free(IntrinsicMap* map);

void IntrinsicMap_set(IntrinsicMap* map, String key, Atom value);
Atom IntrinsicMap_get(IntrinsicMap* map, String key);

bool IntrinsicMap_exists(IntrinsicMap* map, String key);

#endif//MERIDIAN_INTRINSICS_H
