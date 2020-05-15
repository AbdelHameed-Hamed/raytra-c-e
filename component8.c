#include <x86intrin.h>

#define INLINE __attribute__((always_inline))

typedef __m256 Component8f;

typedef struct {
	Component8f xs, ys, zs;
} IVec3x8, *Vec3x8;

INLINE static IVec3x8
vec3x8_add(const Vec3x8 lhs, const Vec3x8 rhs) {
	IVec3x8 res;

	res.xs = lhs->xs + rhs->xs;
	res.ys = lhs->ys + rhs->ys;
	res.zs = lhs->zs + rhs->zs;

	return res;
}

INLINE static Component8f
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

INLINE static Component8f
vec3x8_mag_sqr(const Vec3x8 self) {
	return vec3x8_dot(self, self);
}

INLINE static Component8f
vec3x8_mag(const Vec3x8 self) {
	return _mm256_sqrt_ps(vec3x8_dot(self, self));
}

INLINE static IVec3x8
vec3x8_unit(const Vec3x8 self) {
	IVec3x8 res;

	Component8f mag = vec3x8_mag(self);

	res.xs = self->xs / mag;
	res.ys = self->ys / mag;
	res.zs = self->zs / mag;

	return res;
}

int main() {
	return 0;
}
