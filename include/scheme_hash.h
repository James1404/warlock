#ifndef SCHEME_HASH_H
#define SCHEME_HASH_H

#include "scheme_common.h"

typedef usize Hash;

Hash Hash_string(char* str, u64 len);
Hash Hash_i32(i32 v);

#endif//SCHEME_HASH_H
