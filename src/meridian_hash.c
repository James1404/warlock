#include "meridian_hash.h"

#include <stdlib.h>

//
// --- Hash Functions ---
//
DEFINE_HASH_FN(String, opaque) {
    String* value = CAST_OPAQUE(opaque, String);

    u32 b = 378551;
    u32 a = 63689;

    Hash hash = 0;

    for(i32 i = 0; i < value->length; i++) {
        char c = value->data[i];
        hash = hash * a + c;
        a = a * b;
    }

   return hash;
}

DEFINE_HASH_FN(i32, opaque) {
    i32* value = CAST_OPAQUE(opaque, i32);
    return (usize)*value;
}

DEFINE_HASH_FN(u32, opaque) {
  u32* value = CAST_OPAQUE(opaque, u32);
  return (usize)*value;
}

//
// --- Hash Map ---
//

static void HashMap_realloc(HashMap* hashmap) {
    if(!hashmap->buckets) {
        hashmap->buckets = calloc(hashmap->allocated, sizeof(Atom));
        return;
    }

    hashmap->buckets = realloc(hashmap->buckets, hashmap->allocated * sizeof(Atom));
}

#define MAX_LOADFACTOR 0.75f
static f32 HashMap_loadfactor(HashMap* hashmap) {
    return (f32)hashmap->length / (f32)hashmap->allocated;
}

static HashMapEntry* HashMap_find_entry(HashMap* hashmap, Hash hash) {
    Hash idx = hash % hashmap->allocated;

    HashMapEntry* entry = hashmap->buckets + idx;

    while(entry->next) {
        if(entry->hash == hash) break;
        entry = entry->next;
    }

    return entry;
}

HashMap HashMap_make(void) {
    return (HashMap) {
        .buckets = NULL,
        .allocated = 100,
        .length = 0,
    };
}

void HashMap_free(HashMap* hashmap) {
    if(hashmap->buckets) free(hashmap->buckets);
    *hashmap = HashMap_make();
}

void HashMap_set(HashMap* hashmap, String key, Atom value) {
    if(!hashmap->buckets) {
        HashMap_realloc(hashmap);
    }

    if(HashMap_loadfactor(hashmap) >= MAX_LOADFACTOR) {
        HashMap_realloc(hashmap);
    }

    Hash hash = GET_HASH_FN(String, &key);

    HashMapEntry* entry = HashMap_find_entry(hashmap, hash);
    if(entry->hash != hash) hashmap->length++;

    *entry = (HashMapEntry) {
        .hash = hash,
        .key = key,
        .value = value,
        .next = NULL
    };
}

Atom HashMap_get(HashMap* hashmap, String key) {
    Hash idx = GET_HASH_FN(String, &key) % hashmap->allocated;
    return hashmap->buckets[idx].value;
}

//
// --- Hash Set ---
//

static void HashSet_realloc(HashSet* hashset) {
    if(!hashset->buckets) {
        hashset->buckets = calloc(hashset->allocated, sizeof(Atom));
        return;
    }

    hashset->buckets = realloc(hashset->buckets, hashset->allocated * sizeof(Atom));
}

static f32 HashSet_loadfactor(HashSet* hashset) {
    return (f32)hashset->length / (f32)hashset->allocated;
}

static HashSetEntry* HashSet_find_entry(HashSet* hashset, Hash hash) {
    Hash idx = hash % hashset->allocated;

    HashSetEntry* entry = hashset->buckets + idx;

    while(entry->next) {
        if(entry->hash == hash) break;
        entry = entry->next;
    }

    return entry;
}

HashSet HashSet_make(void) {
    return (HashSet) {
        .buckets = NULL,
        .allocated = 100,
        .length = 0,
    };
}
void HashSet_free(HashSet* hashset) {
    if(hashset->buckets) free(hashset->buckets);
    *hashset = HashSet_make();
}

void HashSet_add(HashSet* hashset, String key) {
    if(!hashset->buckets) {
        HashSet_realloc(hashset);
    }

    if(HashSet_loadfactor(hashset) >= MAX_LOADFACTOR) {
        HashSet_realloc(hashset);
    }

    Hash hash = GET_HASH_FN(String, &key);

    HashSetEntry* entry = HashSet_find_entry(hashset, hash);

    *entry = (HashSetEntry) {
        .hash = hash,
        .key = key,
        .next = NULL
    };
}

bool HashSet_has(HashSet* hashset, String key) {
    Hash hash = GET_HASH_FN(String, &key);

    HashSetEntry* entry = HashSet_find_entry(hashset, hash);

    return entry->hash == hash;
}
