#include <x86intrin.h>
#include <inttypes.h> // ToDo: Check if this is really needed.

#define INLINE __attribute__((always_inline))

typedef __m256 IComponent8f, *Component8f;
typedef u32 uint32x8 __attribute__((__vector_size__(32), __aligned__(32)));

_Thread_local static uint32x8 rnd_states8 = {
	1		 , 268476417, 1157628417, 1158709409,
	269814307, 672445067, 2022772137, 1505329467
};

// 56940020 last number in that list.

INLINE static uint32x8
xor_shift32() {
	uint32x8 x = rnd_states8;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 15;
	rnd_states8 = x;
	return x;
}

typedef struct {
	IComponent8f xs, ys, zs;
} IVec3x8, *Vec3x8;

INLINE static IVec3x8
vec3x8_add(const Vec3x8 lhs, const Vec3x8 rhs) {
	IVec3x8 res;

	res.xs = lhs->xs + rhs->xs;
	res.ys = lhs->ys + rhs->ys;
	res.zs = lhs->zs + rhs->zs;

	return res;
}

INLINE static IComponent8f
vec3x8_dot(const Vec3x8 lhs, const Vec3x8 rhs) {
	return lhs->xs * rhs->xs + lhs->ys * rhs->ys + lhs->zs * rhs->zs;
}

INLINE static IVec3x8
vec3x8_cross(const Vec3x8 lhs, const Vec3x8 rhs) {
	IVec3x8 res;

	res.xs = lhs->ys * rhs->zs - lhs->zs * rhs->ys;
	res.ys = lhs->zs * rhs->xs - lhs->xs * rhs->zs;
	res.xs = lhs->xs * rhs->ys - lhs->ys * rhs->xs;

	return res;	
}

INLINE static IComponent8f
vec3x8_mag_sqr(const Vec3x8 self) {
	return vec3x8_dot(self, self);
}

INLINE static IComponent8f
vec3x8_mag(const Vec3x8 self) {
	return _mm256_sqrt_ps(vec3x8_dot(self, self));
}

INLINE static IVec3x8
vec3x8_unit(const Vec3x8 self) {
	IVec3x8 res;

	IComponent8f mag = vec3x8_mag(self);

	res.xs = self->xs / mag;
	res.ys = self->ys / mag;
	res.zs = self->zs / mag;

	return res;
}

#include <stdio.h>

int main() {
	volatile IComponent8f test1_c = {1, 2, 3, 4, 5, 6, 7, 8}, test2_c = {9, 2, 3, 4, 5, 6, 7, 8};
	volatile IVec3x8 test1_v = {test1_c, test1_c, test1_c}, test2_v = {test2_c, test2_c, test2_c};

	// volatile IComponent8f test3_c = vec3x8_dot(&test1_v, &test2_v);

	volatile IVec3x8 test3_v = vec3x8_add(&test1_v, &test2_v);

	// printf("%f, %f, %f, %f, %f, %f, %f, %f\n", test3[0], test3[1], test3[2], test3[3], test3[4], test3[5], test3[6], test3[7]);

	return 0;
}
