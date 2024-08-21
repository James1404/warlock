#ifndef WARLOCK_HASH_H
#define WARLOCK_HASH_H

#include "warlock_common.h"

typedef usize Hash;

Hash Hash_string(char* str, u64 len);
Hash Hash_i32(i32 v);

#endif//WARLOCK_HASH_H
