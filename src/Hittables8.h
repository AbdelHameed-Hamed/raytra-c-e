#pragma once

#include "component8.c"

typedef struct {
	IVec3x8 ps, normals;
	IComponent8f t;
} IHit_Records8, *Hit_Records8;
