#pragma once

#include "vec3.c"

#include <stdlib.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288f)
#endif

__attribute__((always_inline)) static float
random_float() {
	// Might wanna change this later.
	return rand() / (RAND_MAX + 1.0);
}

__attribute__((always_inline)) static float
random_float_in_range(float min, float max) {
	return min + (max - min) * random_float();
}

typedef struct {
	Vec3 origin;
	Vec3 direction;
} Ray;

static Vec3
ray_point_at_parameter(const Ray self, float t) {
	return self.origin + t * self.direction;
}
