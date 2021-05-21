#pragma once

#include <stdint.h>

#include "tracy-0.7.8\TracyC.h"

// Unsigned
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Signed
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// Floating-point
typedef float f32;
typedef double f64;

#define INLINE __attribute__((always_inline))

#define thread_local _Thread_local

// This isn't nice, first off it's Clang only, second, it binds local vars as const copies, this can be
// problematic and cause leaks/incorrect behaviour.
inline static void defer_cleanup(void (^*b)(void)) { (*b)(); }
#define defer_merge(a,b) a##b
#define defer_varname(a) defer_merge(defer_scopevar_, a)
#define defer __attribute__((cleanup(defer_cleanup))) void (^defer_varname(__COUNTER__))(void) = ^

// I'd use this one, but it's annoying since it forces me to encamposs my code in curly brackets and indent
// #define concat(a, b) a##b
// #define unique(name) concat(name, __LINE__)
// #define defer(end) for (int unique(_i_) = 0; !unique(_i_); (unique(_i_) += 1), (end))
