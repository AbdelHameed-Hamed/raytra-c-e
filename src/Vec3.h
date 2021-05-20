#pragma once

#include <math.h>

#include "Base.h"

typedef float Vec3 __attribute__((ext_vector_type(3)));

__vectorcall INLINE static float
vec3_dot(const Vec3 lhs, const Vec3 rhs)
{
    Vec3 temp = lhs * rhs;
    float res = temp.x + temp.y + temp.z;

    return res;
}

__vectorcall INLINE static Vec3
vec3_cross(const Vec3 lhs, const Vec3 rhs)
{
    return lhs.yzx * rhs.zxy - lhs.zxy * rhs.yzx;
}

__vectorcall INLINE static float
vec3_mag_sqr(const Vec3 self)
{
    return vec3_dot(self, self);
}

__vectorcall INLINE static float
vec3_mag(const Vec3 self)
{
    return sqrtf(vec3_mag_sqr(self));
}

__vectorcall INLINE static Vec3
vec3_unit(const Vec3 self)
{
    return self / vec3_mag(self);
}
