#pragma once

#include "Component8.c"

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288f)
#endif

typedef struct {
	// Vec3 origin;
	// Vec3 direction;
	IVec3x8 origins, directions;
} IRays8, *Rays8;

INLINE static IVec3x8
ray_point_at_parameter(const Rays8 self, Component8f t) {
	IVec3x8 res;

	res.xs = self->origins.xs + (*t) * self->directions.xs;
	res.ys = self->origins.ys + (*t) * self->directions.ys;
	res.zs = self->origins.zs + (*t) * self->directions.zs;

	return res;
}
