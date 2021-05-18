#pragma once

#include "Ray.h"

typedef struct {
    Vec3 p;
    Vec3 normal;
    float t;
} IHit_Record, *Hit_Record;
