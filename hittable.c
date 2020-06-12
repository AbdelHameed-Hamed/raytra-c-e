#pragma once

#include "ray.c"

typedef struct {
	Vec3 p;
	Vec3 normal;
	float t;
} IHit_Record, *Hit_Record;
