#include "meridian_hash.h"

Hash Hash_string(char* str, u64 len) {
    u32 b = 378551;
    u32 a = 63689;

    Hash hash = 0;

    for(i32 i = 0; i < len; i++) {
        char c = str[i];
        hash = hash * a + c;
        a = a * b;
    }

   return hash;
}

Hash Hash_i32(i32 v) {
    return (usize)v;
}
