#ifndef MERIDIAN_HASH_H
#define MERIDIAN_HASH_H

#include "meridian_common.h"

#include "meridian_atom.h"

typedef usize Hash;

#define DEFINE_HASH_FN(T, p) Hash hash_##T(void* opaque)
#define GET_HASH_FN(T, v) hash_##T((void*)v)

#define CAST_OPAQUE(v, T) (T*)v

DEFINE_HASH_FN(String, value);
DEFINE_HASH_FN(i32, value);

typedef Hash (*HashFn)(void* opaque);

//
// --- Hash Map ---
//
typedef struct HashMapEntry {
    Hash hash;

    String key;
    Atom value;

    struct HashMapEntry* next;
} HashMapEntry;

typedef struct {
    usize allocated, length;
    HashMapEntry* buckets;
} HashMap;

HashMap HashMap_make(void);
void HashMap_free(HashMap* hashmap);

void HashMap_set(HashMap* hashmap, String key, Atom value);
Atom HashMap_get(HashMap* hashmap, String key);

Atom HashMap_exists(HashMap* hashmap, String key);

//
// --- Hash Set ---
//

typedef struct HashSetEntry {
    Hash hash;
    String key;

    struct HashSetEntry* next;
} HashSetEntry;

typedef struct {
    usize allocated, length;
    HashSetEntry* buckets;
} HashSet;

HashSet HashSet_make(void);
void HashSet_free(HashSet* hashset);

void HashSet_add(HashSet* hashset, String key);
bool HashSet_has(HashSet* hashset, String key);

#endif//MERIDIAN_HASH_H
