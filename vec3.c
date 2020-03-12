#include <math.h>
#include <string.h>

typedef float IVec3 __attribute__((ext_vector_type(4)));

typedef IVec3 Vec3;

static Vec3 vec3_init() { return (Vec3){ 0.0f }; }

static float vec3_dot(const Vec3 lhs, const Vec3 rhs) {
	float res = lhs[0] * rhs[0] +
				lhs[1] * rhs[1] +
				lhs[2] * rhs[2];

	return res;
}

static Vec3 vec3_cross(const Vec3 lhs, const Vec3 rhs) {
	Vec3 res = vec3_init();
	res[0] = (lhs[1] * rhs[2]) - (lhs[2] * rhs[1]);
	res[1] = (lhs[2] * rhs[0]) - (lhs[0] * rhs[2]);
	res[2] = (lhs[0] * rhs[1]) - (lhs[1] * rhs[0]);

	return res;
}

static Vec3 vec3_cross_inplace(Vec3 lhs, const Vec3 rhs) {
	lhs[0] = (lhs[1] * rhs[2]) - (lhs[2] * rhs[1]);
	lhs[1] = (lhs[2] * rhs[0]) - (lhs[0] * rhs[2]);
	lhs[2] = (lhs[0] * rhs[1]) - (lhs[1] * rhs[0]);

	return lhs;
}

static float vec3_mag_sqr(const Vec3 self) {
	return self[0] * self[0] +
		   self[1] * self[1] +
		   self[2] * self[2];
}

static float vec3_mag(const Vec3 self) {
	return sqrtf(vec3_mag_sqr(self));
}

static Vec3 vec3_make_unit(Vec3 self) {
	float mag = vec3_mag(self);

	self.xyz /= mag;
	self.xyz /= mag;
	self.xyz /= mag;

	return self;
}

static Vec3 vec3_unit(const Vec3 self) {
	float mag = vec3_mag(self);
	return self / mag;
}
