#pragma once

#include "Ray.h"
#include "Rays8.h"

INLINE static Vec3
random_vec3_in_unit_disk()
{
    Vec3 res;
    do {
        res = 2.0f * (Vec3){random_float(), random_float(), 0.0f} - (Vec3){1.0f, 1.0f, 0.0f};
    } while (vec3_mag_sqr(res) >= 1.0f);

    return res;
}

const Vec3x8 unit_desk_vec = {
    .xs = {1, 1, 1, 1, 1, 1, 1, 1},
    .ys = {1, 1, 1, 1, 1, 1, 1, 1},
    .zs = {0}
};

INLINE static Vec3x8
random_vec3x8_in_unit_disk()
{
    f32x8 mask = {0};
    Vec3x8 ps = {0};
    while (!is_mask_saturated(&mask))
    {
        ps = vec3x8_blend(
            vec3x8_sub(
                vec3x8_mul_f32((Vec3x8){random_f32x8(), random_f32x8(), zeros}, 2),
                vec3x8_from_vec3((Vec3){1, 1, 0})
            ),
            ps,
            mask
        );
        mask = _mm256_cmp_ps(vec3x8_mag_sqr(ps), ones, _CMP_LT_OQ);
    }

    return ps;
}

typedef struct {
    Vec3 lower_left_corner;
    Vec3 horizental;
    Vec3 vertical;
    Vec3 origin;
    Vec3 u, v, w;
    float lens_radius;
} Camera;

__vectorcall INLINE static Camera
camera_new(
    Vec3 look_from,
    Vec3 look_at,
    Vec3 v_up,
    float vfov,
    float aspect,
    float aperture,
    float focus_dist)
{
    Camera self;

    float theta       = vfov * M_PI / 180.0f;
    float half_height = tanf(theta / 2.0f);
    float half_width  = aspect * half_height;
    self.lens_radius  = aperture / 2.0f;

    self.w = vec3_unit(look_from - look_at);
    self.u = vec3_unit(vec3_cross(v_up, self.w));
    self.v = vec3_cross(self.w, self.u);

    self.origin            = look_from;
    self.lower_left_corner = self.origin -
                             half_width * focus_dist * self.u -
                             half_height * focus_dist * self.v -
                             focus_dist * self.w;
    self.horizental        = 2 * half_width * focus_dist * self.u;
    self.vertical          = 2 * half_height * focus_dist * self.v;

    return self;
}

__vectorcall INLINE static Ray
camera_get_ray(const Camera self, float u, float v)
{
    Vec3 rd     = self.lens_radius * random_vec3_in_unit_disk();
    Vec3 offset = self.u * rd.x + v * rd.y;

    return (Ray){
        self.origin + offset,
        self.horizental * u + self.lower_left_corner + self.vertical * v - self.origin - offset
    };
}

__vectorcall INLINE static Rays8
camera_get_rays8(const Camera self, const f32x8 u, const f32x8 v)
{
    Vec3x8 rds = vec3x8_mul_f32(random_vec3x8_in_unit_disk(), self.lens_radius);
    Vec3x8 offsets = vec3x8_add(
        vec3x8_mul_f32x8(vec3x8_from_vec3(self.u), rds.xs),
        vec3x8_mul_f32x8(vec3x8_from_vec3(self.v), rds.ys)
    );

    return (Rays8){
        vec3x8_add(vec3x8_from_vec3(self.origin), offsets),
        vec3x8_sub(
            vec3x8_sub(
                vec3x8_add(
                    vec3x8_add(
                        vec3x8_mul_f32x8(vec3x8_from_vec3(self.horizental), u),
                        vec3x8_from_vec3(self.lower_left_corner)
                    ),
                    vec3x8_mul_f32x8(vec3x8_from_vec3(self.vertical), v)
                ),
                vec3x8_from_vec3(self.origin)
            ),
            offsets
        )
    };
}
