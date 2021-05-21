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
