#ifndef MERIDIAN_HASH_H
#define MERIDIAN_HASH_H

#include "scheme_common.h"

typedef usize Hash;

Hash Hash_string(char* str, u64 len);
Hash Hash_i32(i32 v);

#endif//MERIDIAN_HASH_H
