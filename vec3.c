#include <math.h>

typedef float Vec3 __attribute__((ext_vector_type(3)));

static float
vec3_dot(const Vec3 lhs, const Vec3 rhs) {
	Vec3 temp = lhs * rhs;
	float res = temp.x + temp.y + temp.z;

	return res;
}

static Vec3
vec3_cross(const Vec3 lhs, const Vec3 rhs) { 
	return lhs.yzx * rhs.zxy - lhs.zxy * rhs.yzx;
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
vec3_unit(const Vec3 self) {
	return self / vec3_mag(self);
}
