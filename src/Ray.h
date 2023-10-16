#pragma once

#include <stdlib.h>

#include "Vec3.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288f)
#endif

// Note: Should probably deal with multithreading stuff in a better way...
thread_local u32 seed = 7;

INLINE static u32
xor_shift32()
{
    u32 x = seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 15;
    seed = x;

    return x;
}

thread_local struct {
    u64 state;
    u64 inc;
} rng = { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL };

INLINE static u32
pcg32_random_r()
{
    u64 oldstate = rng.state;
    // Advance internal state
    rng.state = oldstate * 6364136223846793005ULL + (rng.inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    u32 xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    u32 rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

INLINE static float
random_float()
{
    // Might wanna change this later.
    return pcg32_random_r() / (float)UINT32_MAX;
}

__vectorcall INLINE static float
random_float_in_range(float min, float max)
{
    return min + (max - min) * random_float();
}

typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

__vectorcall INLINE static Vec3
ray_point_at_parameter(const Ray self, float t)
{
    return self.origin + t * self.direction;
}
