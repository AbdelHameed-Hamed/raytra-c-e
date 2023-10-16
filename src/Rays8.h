#pragma once

#include "Vec3x8.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288f)
#endif

typedef struct {
    Vec3x8 origins, directions;
} Rays8;

__vectorcall INLINE static Vec3x8
rays8_point_at_parameter(const Rays8 self, const f32x8 t) {
    return (Vec3x8){
        self.origins.xs + t * self.directions.xs,
        self.origins.ys + t * self.directions.ys,
        self.origins.zs + t * self.directions.zs
    };
}
