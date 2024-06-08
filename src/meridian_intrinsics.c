#include "meridian_intrinsics.h"

IntrinsicMap IntrinsicMap_make() {
    return (IntrinsicMap) {
        .length = 0,
        .allocated = 0,
        .buckets = NULL,
    };
}

void IntrinsicMap_free(IntrinsicMap* map) {
}

void IntrinsicMap_set(IntrinsicMap* map, String key, Atom value) {
}

Atom IntrinsicMap_get(IntrinsicMap* map, String key) {
}

bool IntrinsicMap_exists(IntrinsicMap* map, String key) {
}
