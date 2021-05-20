#pragma once

#include <stdbool.h>

#include "Materials.h"

typedef struct {
    Material material;
    Vec3 center;
    float radius;
} Sphere;

__vectorcall INLINE static _Bool
sphere_hit(const Sphere self, const Ray r, float t_min, float t_max, Hit_Record *rec)
{
    Vec3 oc = r.origin - self.center;
    float a = vec3_mag_sqr(r.direction);
    float b = vec3_dot(oc, r.direction);
    float c = vec3_mag_sqr(oc) - self.radius * self.radius;
    float discriminant = b * b - a * c;

    if (discriminant > 0)
    {
        float temp = (-b - sqrtf(discriminant)) / a;
        if (temp < t_max && temp > t_min)
        {
            rec->t      = temp;
            rec->p      = ray_point_at_parameter(r, rec->t);
            rec->normal = (rec->p - self.center) / self.radius;
            return 1;
        }

        temp = (-b + sqrtf(discriminant)) / a;
        if (temp < t_max && temp > t_min)
        {
            rec->t      = temp;
            rec->p      = ray_point_at_parameter(r, rec->t);
            rec->normal = (rec->p - self.center) / self.radius;
            return 1;
        }
    }

    return 0;
}
