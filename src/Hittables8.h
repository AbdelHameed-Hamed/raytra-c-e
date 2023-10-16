#pragma once

#include "Vec3x8.h"

typedef struct {
    Vec3x8 ps, normals;
    f32x8 ts;
} Hit_Records8;
