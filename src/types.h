#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef int8_t   i8; // -128 + 127
typedef int16_t  i16; // -32k +32k
typedef int32_t  i32; // -2billion + 2billion
typedef int64_t  i64; // stupid large numbers

// Same as above just starting from zero, so MAX is double (no negative)
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef struct {
  char *data;
  size_t unitCount;
  size_t capacity;
} String;

#endif
