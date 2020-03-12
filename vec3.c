#include <math.h>
#include <string.h>

typedef float IVec3 __attribute__((ext_vector_type(3)));

typedef IVec3 Vec3;

static float
vec3_dot(const Vec3 lhs, const Vec3 rhs) {
	Vec3 temp = lhs * rhs;
	float res = temp[0] + temp[1] + temp[2];

	return res;
}

static Vec3
vec3_cross(const Vec3 lhs, const Vec3 rhs) { 
	return lhs.yzx * rhs.zxy - lhs.zxy * rhs.yzx;
}

static Vec3
vec3_cross_inplace(Vec3 lhs, const Vec3 rhs) {
	lhs = lhs.yzx * rhs.zxy - lhs.zxy * rhs.yzx;

	return lhs;
}

static float
vec3_mag_sqr(const Vec3 self) {
	return vec3_dot(self, self);
}

static float
vec3_mag(const Vec3 self) {
	return sqrtf(vec3_mag_sqr(self));
}

static Vec3
vec3_make_unit(Vec3 self) {
	float mag = vec3_mag(self);
	self.xyz /= mag;

	return self;
}

static Vec3
vec3_unit(const Vec3 self) {
	float mag = vec3_mag(self);
	return self / mag;
}
