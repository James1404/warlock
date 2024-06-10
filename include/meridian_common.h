#ifndef MERIDIAN_COMMON_H
#define MERIDIAN_COMMON_H

#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef char            byte;
typedef unsigned char   ubyte;

typedef float f32;
typedef long double f64;

typedef size_t usize;

#define U8_MAX  UCHAR_MAX
#define U16_MAX USHRT_MAX
#define U32_MAX ULONG_MAX
#define U64_MAX ULLONG_MAX

#define I8_MAX  CHAR_MAX
#define I16_MAX SHRT_MAX
#define I32_MAX LONG_MAX
#define I64_MAX LLONG_MAX

//
// --- String ---
//

typedef struct {
    byte* data;
    i32 length;
} String;

String String_make(const byte* raw);

byte String_index(String str, i32 pos);
String String_substr(String other, i32 pos, i32 length);

String String_concat_with_raw(String lhs, const char* rhs);
String String_concat(String lhs, String rhs);

const byte* String_get_raw(String str);

bool String_is(String str, const char* cmp);
bool String_cmp(String str, String other);

//
// --- Dynamic Array ---
//

typedef struct {
    byte* data;
    u64 element_size;

    u64 capacity, length;
} DynamicArray;

DynamicArray DynamicArray_make(u64 element_size);
void DynamicArray_free(DynamicArray* array);

void DynamicArray_push(DynamicArray* array, void* elem);

#define DynamicArray_at(T, array, i) (T*)((array).data + i)

//
// --- Page Allocator ---
//

typedef struct {
    u64 allocated, used;
    u8* data;
} Region;

typedef struct {
    u64 size, length;
    Region* regions;
} ArenaAllocator;

ArenaAllocator ArenaAllocator_make(u64 region_size);
void ArenaAllocator_free(ArenaAllocator* allocator);

void* ArenaAllocator_alloc(ArenaAllocator* allocator, u64 size);

#endif//MERIDIAN_COMMON_H
