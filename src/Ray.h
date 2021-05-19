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

INLINE static float
random_float()
{
    // Might wanna change this later.
    return xor_shift32() / (float)UINT32_MAX;
}

INLINE static float
random_float_in_range(float min, float max)
{
    return min + (max - min) * random_float();
}

typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

INLINE static Vec3
ray_point_at_parameter(const Ray self, float t)
{
    return self.origin + t * self.direction;
}
