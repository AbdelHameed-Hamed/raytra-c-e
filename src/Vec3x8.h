#pragma once

#include <immintrin.h>
#include <string.h>

#include "Base.h"
#include "Vec3.h"

typedef f32 f32x8 __attribute__((__vector_size__(32), __aligned__(32)));
typedef u32 u32x8 __attribute__((__vector_size__(32), __aligned__(32)));
typedef u64 u64x4 __attribute__((__vector_size__(32), __aligned__(32)));

const f32x8 ones = {1, 1, 1, 1, 1, 1, 1, 1};
const f32x8 zeros = {0, 0, 0, 0, 0, 0, 0, 0};
// Initialized in main.
f32x8 full_mask;

__vectorcall INLINE _Bool
is_mask_saturated(const f32x8 *mask) {
    return memcmp(mask, &full_mask, sizeof(f32x8)) == 0;
}

__vectorcall INLINE _Bool
is_mask_cleared(const f32x8 mask) {
    return _mm256_testz_ps(mask, mask);
}

typedef struct {
    f32x8 xs, ys, zs;
} Vec3x8;

__vectorcall INLINE static Vec3x8
vec3x8_from_vec3(const Vec3 other) {
    return (Vec3x8){
        _mm256_set1_ps(other.x),
        _mm256_set1_ps(other.y),
        _mm256_set1_ps(other.z),
    };
}

__vectorcall INLINE static Vec3x8
vec3x8_from_f32x8(const f32x8 other) {
    return (Vec3x8){
        other,
        other,
        other,
    };
}

__vectorcall INLINE static Vec3x8
vec3x8_add(const Vec3x8 lhs, const Vec3x8 rhs) {
    return (Vec3x8){
        lhs.xs + rhs.xs,
        lhs.ys + rhs.ys,
        lhs.zs + rhs.zs
    };
}

__vectorcall INLINE static Vec3x8
vec3x8_sub(const Vec3x8 lhs, const Vec3x8 rhs) {
    return (Vec3x8){
        lhs.xs - rhs.xs,
        lhs.ys - rhs.ys,
        lhs.zs - rhs.zs
    };
}

__vectorcall INLINE static Vec3x8
vec3x8_mul_f32(const Vec3x8 lhs, const f32 rhs) {
    return (Vec3x8){
        lhs.xs * rhs,
        lhs.ys * rhs,
        lhs.zs * rhs
    };
}

__vectorcall INLINE static Vec3x8
vec3x8_div_f32(const Vec3x8 lhs, const f32 rhs) {
    return (Vec3x8){
        lhs.xs / rhs,
        lhs.ys / rhs,
        lhs.zs / rhs
    };
}

__vectorcall INLINE static Vec3x8
vec3x8_mul_f32x8(const Vec3x8 lhs, const f32x8 rhs) {
    return (Vec3x8){
        lhs.xs * rhs,
        lhs.ys * rhs,
        lhs.zs * rhs
    };
}

__vectorcall INLINE static Vec3x8
vec3x8_div_f32x8(const Vec3x8 lhs, const f32x8 rhs) {
    return (Vec3x8){
        lhs.xs / rhs,
        lhs.ys / rhs,
        lhs.zs / rhs
    };
}

__vectorcall INLINE static Vec3x8
vec3x8_mul_vec3x8(const Vec3x8 lhs, const Vec3x8 rhs) {
    return (Vec3x8){
        lhs.xs * rhs.xs,
        lhs.ys * rhs.ys,
        lhs.zs * rhs.zs
    };
}

__vectorcall INLINE static f32x8
vec3x8_dot(const Vec3x8 lhs, const Vec3x8 rhs) {
    return lhs.xs * rhs.xs + lhs.ys * rhs.ys + lhs.zs * rhs.zs;
}

__vectorcall INLINE static Vec3x8
vec3x8_cross(const Vec3x8 lhs, const Vec3x8 rhs) {
    return (Vec3x8){
        lhs.ys * rhs.zs - lhs.zs * rhs.ys,
        lhs.zs * rhs.xs - lhs.xs * rhs.zs,
        lhs.xs * rhs.ys - lhs.ys * rhs.xs
    };
}

__vectorcall INLINE static f32x8
vec3x8_mag_sqr(const Vec3x8 self) {
    return vec3x8_dot(self, self);
}

__vectorcall INLINE static f32x8
vec3x8_mag(const Vec3x8 self) {
    return _mm256_sqrt_ps(vec3x8_dot(self, self));
}

__vectorcall INLINE static Vec3x8
vec3x8_unit(const Vec3x8 self) {
    f32x8 mag = vec3x8_mag(self);

    return (Vec3x8){
        self.xs / mag,
        self.ys / mag,
        self.zs / mag
    };
}

__vectorcall INLINE static Vec3x8
vec3x8_blend(const Vec3x8 a, const Vec3x8 b, const f32x8 mask) {
    return (Vec3x8){
        _mm256_blendv_ps(a.xs, b.xs, mask),
        _mm256_blendv_ps(a.ys, b.ys, mask),
        _mm256_blendv_ps(a.zs, b.zs, mask),
    };
}

thread_local struct {
    u64x4 state0, state1;
    u64x4 inc0, inc1;
} rngs8 = {
    .state0 = {0xb5f380a45f908741, 0x88b545898d45385d, 0xd81c7fe764f8966c, 0x44a9a3b6b119e7bc},
    .state1 = {0x3cb6e04dc22f629, 0x727947debc931183, 0xfbfa8fdcff91891f, 0xb9384fd8f34c0f49},
    .inc0 = {0xbf2de0670ac3d03f, 0x98c40c0dc94e71f, 0xf3565f35a8c61d01, 0xd3c83e29b30df641},
    .inc1 = {0x14b7f6e4c89630fb, 0x37cc7b0347694551, 0x4a052322d95d485b, 0x10f3ade77a26e15f}
};

INLINE static u32x8
pcg32x8_random_r() {
    const u64x4 mask_l = _mm256_set1_epi64x(0x00000000ffffffffull);
    const u64x4 oldstate0 = rngs8.state0, oldstate1 = rngs8.state1;
    rngs8.state0 = oldstate0 * 6364136223846793005ULL + rngs8.inc0;
    rngs8.state1 = oldstate1 * 6364136223846793005ULL + rngs8.inc1;

    const u32x8 shift0 = {7, 7, 7, 7, 6, 4, 2, 0};
    const u32x8 shift1 = {6, 4, 2, 0, 7, 7, 7, 7};

    const u64x4 xorshifted0 = _mm256_and_si256(((oldstate0 >> 18u) ^ oldstate0) >> 27u, mask_l);
    const u64x4 xorshifted1 = _mm256_and_si256((((oldstate1 >> 18u) ^ oldstate1) >> 27u), mask_l);
    const u64x4 rot0 = oldstate0 >> 59u;
    const u64x4 rot1 = oldstate1 >> 59u;

    const u32x8 xorshifted2 = _mm256_permutevar8x32_epi32(xorshifted0, shift0);
    const u32x8 xorshifted3 = _mm256_permutevar8x32_epi32(xorshifted1, shift1);
    const u32x8 rot2 = _mm256_permutevar8x32_epi32(rot0, shift0);
    const u32x8 rot3 = _mm256_permutevar8x32_epi32(rot1, shift1);

    const u32x8 xorshifted = _mm256_or_si256(xorshifted2, xorshifted3);
    const u32x8 rot = _mm256_or_si256(rot2, rot3);

    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31u));
}

INLINE static f32x8
random_f32x8() {
    // Trick from MTGP: generate a uniformly distributed
    // single precision number in [1,2) and subtract 1.
    const __m256i ones = _mm256_set1_epi32(0x3f800000u);

    const u32x8 value = pcg32x8_random_r();
    const u32x8 float_value = _mm256_or_si256(_mm256_srli_epi32(value, 9), ones);

    return _mm256_castsi256_ps(float_value) - _mm256_castsi256_ps(ones);
}

__vectorcall INLINE static f32x8
random_f32x8_in_range(float min, float max) {
    return min + (max - min) * random_f32x8();
}

__vectorcall INLINE static f32x8
f32x8_cos(const f32x8 self) {
    float _0 = cosf(self[0]);
    float _1 = cosf(self[1]);
    float _2 = cosf(self[2]);
    float _3 = cosf(self[3]);
    float _4 = cosf(self[4]);
    float _5 = cosf(self[5]);
    float _6 = cosf(self[6]);
    float _7 = cosf(self[7]);

    return (f32x8){_0, _1, _2, _3, _4, _5, _6, _7};
}

__vectorcall INLINE static f32x8
f32x8_sin(const f32x8 self) {
    float _0 = sinf(self[0]);
    float _1 = sinf(self[1]);
    float _2 = sinf(self[2]);
    float _3 = sinf(self[3]);
    float _4 = sinf(self[4]);
    float _5 = sinf(self[5]);
    float _6 = sinf(self[6]);
    float _7 = sinf(self[7]);

    return (f32x8){_0, _1, _2, _3, _4, _5, _6, _7};
}

INLINE static Vec3x8
random_unit_vec3x8() {
    f32x8 a = random_f32x8_in_range(0.0f, 2.0f * M_PI);
    f32x8 z = random_f32x8_in_range(-1.0f, 1.0f);
    f32x8 r = _mm256_sqrt_ps(1.0f - z * z);

    return (Vec3x8){r * f32x8_cos(a), r * f32x8_sin(a), z};
}
