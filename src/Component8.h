#pragma once

#include <immintrin.h>

#include "Base.h"

typedef __m256 Component8;
typedef u32 uint32x8 __attribute__((__vector_size__(32), __aligned__(32)));

thread_local static uint32x8 rnd_states8 = {
    1        , 268476417, 1157628417, 1158709409,
    269814307, 672445067, 2022772137, 1505329467
};

// 56940020 last number in that list.

INLINE static uint32x8
xor_shift32x8()
{
    uint32x8 x = rnd_states8;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 15;
    rnd_states8 = x;

    return x;
}

typedef struct {
    Component8 xs, ys, zs;
} Vec3x8;

INLINE static Vec3x8
vec3x8_add(const Vec3x8 lhs, const Vec3x8 rhs)
{
    return (Vec3x8){
        lhs.xs + rhs.xs,
        lhs.ys + rhs.ys,
        lhs.zs + rhs.zs
    };
}

INLINE static Component8
vec3x8_dot(const Vec3x8 lhs, const Vec3x8 rhs)
{
    return lhs.xs * rhs.xs + lhs.ys * rhs.ys + lhs.zs * rhs.zs;
}

INLINE static Vec3x8
vec3x8_cross(const Vec3x8 lhs, const Vec3x8 rhs)
{
    return (Vec3x8){
        lhs.ys * rhs.zs - lhs.zs * rhs.ys,
        lhs.zs * rhs.xs - lhs.xs * rhs.zs,
        lhs.xs * rhs.ys - lhs.ys * rhs.xs
    };
}

INLINE static Component8
vec3x8_mag_sqr(const Vec3x8 self)
{
    return vec3x8_dot(self, self);
}

INLINE static Component8
vec3x8_mag(const Vec3x8 self)
{
    return _mm256_sqrt_ps(vec3x8_dot(self, self));
}

INLINE static Vec3x8
vec3x8_unit(const Vec3x8 self)
{
    Component8 mag = vec3x8_mag(self);

    return (Vec3x8){
        self.xs / mag,
        self.ys / mag,
        self.zs / mag
    };
}
