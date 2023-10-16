#pragma once

#include <string.h>

#include "Rays8.h"
#include "Hittables8.h"

typedef struct {
    Vec3 center;
    float radius;
} Seperate_Sphere;

__vectorcall INLINE static f32x8
sphere_hit_rays8(const Seperate_Sphere self, const Rays8 rs, const f32x8 t_min, const f32x8 t_max, Hit_Records8 *recs)
{
    f32x8 res = {0};

    const Vec3x8 center = vec3x8_from_vec3(self.center);
    const f32x8 radius = _mm256_set1_ps(self.radius);

    const Vec3x8 oc = vec3x8_sub(rs.origins, center);
    const f32x8 a = vec3x8_mag_sqr(rs.directions);
    const f32x8 b = vec3x8_dot(oc, rs.directions);
    const f32x8 c = vec3x8_mag_sqr(oc) - radius * radius;
    const f32x8 discriminant = b * b - a * c;

    const f32x8 discriminant_mask = _mm256_cmp_ps(discriminant, zeros, _CMP_GT_OQ);
    if (is_mask_cleared(discriminant_mask))
        return discriminant_mask;
    {
        f32x8 temp = (-b - _mm256_sqrt_ps(discriminant)) / a;
        const f32x8 first_solution_mask = _mm256_and_ps(
            _mm256_and_ps(_mm256_cmp_ps(temp, t_max, _CMP_LT_OQ), _mm256_cmp_ps(temp, t_min, _CMP_GT_OQ)),
            discriminant_mask
        );
        {
            recs->ts = _mm256_blendv_ps(recs->ts, temp, first_solution_mask);
            recs->ps = vec3x8_blend(recs->ps, rays8_point_at_parameter(rs, recs->ts), first_solution_mask);
            recs->normals = vec3x8_blend(
                recs->normals,
                vec3x8_div_f32x8(vec3x8_sub(recs->ps, center), radius),
                first_solution_mask
            );
        }
        if (is_mask_saturated(&first_solution_mask))
            return first_solution_mask;

        temp = (-b + _mm256_sqrt_ps(discriminant)) / a;
        const f32x8 second_solution_mask = _mm256_andnot_ps(
            first_solution_mask,
            _mm256_and_ps(
                _mm256_and_ps(_mm256_cmp_ps(temp, t_max, _CMP_LT_OQ), _mm256_cmp_ps(temp, t_min, _CMP_GT_OQ)),
                discriminant_mask
            )
        );
        {
            recs->ts = _mm256_blendv_ps(recs->ts, temp, second_solution_mask);
            recs->ps = vec3x8_blend(recs->ps, rays8_point_at_parameter(rs, recs->ts), second_solution_mask);
            recs->normals = vec3x8_blend(
                recs->normals,
                vec3x8_div_f32x8(vec3x8_sub(recs->ps, center), radius),
                second_solution_mask
            );
        }

        res = _mm256_or_ps(first_solution_mask, second_solution_mask);
    }

    return res;
}
