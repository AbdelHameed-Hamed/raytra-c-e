#pragma once

#include "vec3.c"

#include <stdlib.h>

static float
random_float() {
	float rand_n = rand() / (float)RAND_MAX;
	rand_n = rand_n == 1.0f ? 0.0f : rand_n;
	return rand_n;
}

typedef struct {
	Vec3 origin;
	Vec3 direction;
} Ray;

static Vec3
ray_point_at_parameter(const Ray self, float t) {
	return self.origin + t * self.direction;
}
