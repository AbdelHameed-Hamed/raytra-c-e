#include <math.h>
#include <stdlib.h>
#include <string.h>

struct IVec3{
	float p[3];
};

typedef struct IVec3* Vec3;

Vec3 vec3_init() { return (Vec3)malloc(sizeof(struct IVec3)); }

void vec3_destroy(Vec3 self) { free(self); }

float vec3_x(Vec3 self) { return self->p[0]; }
float vec3_y(Vec3 self) { return self->p[1]; }
float vec3_z(Vec3 self) { return self->p[2]; }

float vec3_r(Vec3 self) { return self->p[0]; }
float vec3_g(Vec3 self) { return self->p[1]; }
float vec3_b(Vec3 self) { return self->p[2]; }

Vec3 vec3_set_floats(Vec3 lhs, const float rhs[3]) {
	memcpy(lhs->p, rhs, sizeof(struct IVec3));

	return lhs;
}

Vec3 vec3_from_floats(const float other[3]) { return vec3_set_floats(vec3_init(), other); }

Vec3 vec3_set_vec3(Vec3 lhs, const Vec3 rhs) {
	*lhs = *rhs;

	return lhs;
}

Vec3 vec3_from_vec3(const Vec3 other) { return vec3_set_vec3(vec3_init(), other); }

Vec3 vec3_add(const Vec3 lhs, const Vec3 rhs) {
	Vec3 res = vec3_init();
	res->p[0] = lhs->p[0] + rhs->p[0];
	res->p[1] = lhs->p[1] + rhs->p[1];
	res->p[2] = lhs->p[2] + rhs->p[2];

	return res;
}

Vec3 vec3_add_inplace(Vec3 lhs, const Vec3 rhs) {
	lhs->p[0] += rhs->p[0];
	lhs->p[1] += rhs->p[1];
	lhs->p[2] += rhs->p[2];

	return lhs;
}

Vec3 vec3_sub(const Vec3 lhs, const Vec3 rhs) {
	Vec3 res = vec3_init();
	res->p[0] = lhs->p[0] - rhs->p[0];
	res->p[1] = lhs->p[1] - rhs->p[1];
	res->p[2] = lhs->p[2] - rhs->p[2];

	return res;
}

Vec3 vec3_sub_inplace(Vec3 lhs, const Vec3 rhs) {
	lhs->p[0] -= rhs->p[0];
	lhs->p[1] -= rhs->p[1];
	lhs->p[2] -= rhs->p[2];

	return lhs;
}

float vec3_dot(const Vec3 lhs, const Vec3 rhs) {
	float res = lhs->p[0] * rhs->p[0] +
			  	lhs->p[1] * rhs->p[1] +
			  	lhs->p[2] * rhs->p[2];

	return res;
}

Vec3 vec3_cross(const Vec3 lhs, const Vec3 rhs) {
	Vec3 res = vec3_init();
	res->p[0] = (lhs->p[1] * rhs->p[2])	- (lhs->p[2] * rhs->p[1]);
	res->p[1] = (lhs->p[2] * rhs->p[0])	- (lhs->p[0] * rhs->p[2]);
	res->p[2] = (lhs->p[0] * rhs->p[1])	- (lhs->p[1] * rhs->p[0]);

	return res;
}

Vec3 vec3_cross_inplace(Vec3 lhs, const Vec3 rhs) {
	lhs->p[0] = (lhs->p[1] * rhs->p[2])	- (lhs->p[2] * rhs->p[1]);
	lhs->p[1] = (lhs->p[2] * rhs->p[0])	- (lhs->p[0] * rhs->p[2]);
	lhs->p[2] = (lhs->p[0] * rhs->p[1])	- (lhs->p[1] * rhs->p[0]);

	return lhs;
}

Vec3 vec3_mul(const Vec3 lhs, float rhs) {
	Vec3 res = vec3_init();
	res->p[0] = lhs->p[0] * rhs;
	res->p[1] = lhs->p[1] * rhs;
	res->p[2] = lhs->p[2] * rhs;

	return res;
}

Vec3 vec3_mul_inplace(Vec3 lhs, float rhs) {
	lhs->p[0] *= rhs;
	lhs->p[1] *= rhs;
	lhs->p[2] *= rhs;

	return lhs;
}

Vec3 vec3_div(const Vec3 lhs, float rhs) {
	Vec3 res = vec3_init();
	res->p[0] = lhs->p[0] / rhs;
	res->p[1] = lhs->p[1] / rhs;
	res->p[2] = lhs->p[2] / rhs;

	return res;
}

Vec3 vec3_div_inplace(Vec3 lhs, float rhs) {
	lhs->p[0] /= rhs;
	lhs->p[1] /= rhs;
	lhs->p[2] /= rhs;

	return lhs;
}

float vec3_mag(const Vec3 self) {
	return sqrt(self->p[0] * self->p[0] +
				self->p[1] * self->p[1] +
				self->p[2] * self->p[2]);
}

float vec3_mag_sqr(const Vec3 self) {
	return (self->p[0] * self->p[0] +
		    self->p[1] * self->p[1] +
		    self->p[2] * self->p[2]);
}

Vec3 vec3_make_unit(Vec3 self) {
	float mag = vec3_mag(self);

	self->p[0] /= mag;
	self->p[1] /= mag;
	self->p[2] /= mag;

	return self;
}

Vec3 vec3_unit(const Vec3 self) {
	float mag = vec3_mag(self);
	return vec3_div_inplace(vec3_from_vec3(self), mag);
}
