#pragma once

#include <assert.h>

#include "Hittable.h"

__vectorcall INLINE static Vec3
reflect(const Vec3 vec, const Vec3 norm)
{
    return vec - 2 * vec3_dot(vec, norm) * norm;
}

__vectorcall INLINE static _Bool
refract(const Vec3 vec, const Vec3 norm, float ni_over_nt, Vec3* refracted)
{
    Vec3 uv            = vec3_unit(vec);
    float dt           = vec3_dot(uv, norm);
    float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
    if (discriminant > 0.0f)
    {
        *refracted = ni_over_nt * (uv - norm * dt) - norm * sqrtf(discriminant);
        return 1;
    }

    return 0;
}

__vectorcall INLINE static float
schlick(float cos, float ref_idx)
{
    float r_0 = (1 - ref_idx) / (1 + ref_idx);
    r_0 *= r_0;

    return r_0 + (1 - r_0) * powf(1.0f - cos, 5.0f);
}

INLINE static Vec3
random_in_unit_sphere()
{
    Vec3 p = {0};
    do {
        p = 2.0f * (Vec3){random_float(), random_float(), random_float()} - (Vec3){1.0f, 1.0f, 1.0f};
    } while (vec3_mag_sqr(p) >= 1.0f);

    return p;
}

INLINE static Vec3
random_unit_vector()
{
    float a = random_float_in_range(0.0f, 2.0f * M_PI);
    float z = random_float_in_range(-1.0f, 1.0f);
    float r = sqrtf(1.0f - z * z);

    return (Vec3){r * cosf(a), r * sinf(a), z};
}

typedef struct {
    Vec3 albedo;
} Lambertian;

__vectorcall INLINE static _Bool
lambertian_scatter(
    const Lambertian self,
    const Hit_Record *rec,
    Vec3 *attenuation,
    Ray *scattered)
{
    *scattered   = (Ray){rec->p, rec->normal + random_unit_vector()};
    *attenuation = self.albedo;

    return 1;
}

typedef struct {
    Vec3 albedo;
    float fuzz;
} Metal;

__vectorcall INLINE static _Bool
metal_scatter(
    const Metal self,
    const Ray r_in,
    const Hit_Record *rec,
    Vec3 *attenuation,
    Ray *scattered)
{
    Vec3 reflected = reflect(vec3_unit(r_in.direction), rec->normal);
    *scattered     = (Ray){rec->p, reflected + self.fuzz * random_in_unit_sphere()};
    *attenuation   = self.albedo;

    return (vec3_dot(scattered->direction, rec->normal) > 0);
}

typedef struct {
    float ref_idx;
} Dielectric;

__vectorcall INLINE static _Bool
dielectric_scatter(
    const Dielectric self,
    const Ray r_in,
    const Hit_Record *rec,
    Vec3 *attenuation,
    Ray *scattered)
{
    Vec3 outward_normal;
    Vec3 reflected = reflect(r_in.direction, rec->normal);
    float ni_over_nt;
    *attenuation = (Vec3){1.0f, 1.0f, 1.0f};
    Vec3 refracted;
    float reflect_prob;
    float cos;

    if (vec3_dot(r_in.direction, rec->normal) > 0.0f)
    {
        outward_normal = -rec->normal;
        ni_over_nt     = self.ref_idx;
        cos            = self.ref_idx * vec3_dot(r_in.direction, rec->normal) / vec3_mag(r_in.direction);
    }
    else
    {
        outward_normal = rec->normal;
        ni_over_nt     = 1.0f / self.ref_idx;
        cos            = -vec3_dot(r_in.direction, rec->normal) / vec3_mag(r_in.direction);
    }

    if (refract(r_in.direction, outward_normal, ni_over_nt, &refracted))
    {
        reflect_prob = schlick(cos, self.ref_idx);
    }
    else
    {
        *scattered = (Ray){rec->p, reflected};
        reflect_prob = 1.0f;
    }

    if (random_float() < reflect_prob)
        *scattered = (Ray){rec->p, reflected};
    else
        *scattered = (Ray){rec->p, refracted};

    return 1;
}

typedef struct {
    union {
        Metal metal;
        Lambertian lambertian;
        Dielectric dielectric;
    } materials;

    enum {
        KIND_METAL,
        KIND_LAMBERTIAN,
        KIND_DIELECTRIC
    } MATERIAL_KIND;
} Material;

__vectorcall INLINE static _Bool
material_scatter(
    Material self,
    const Ray r_in,
    const Hit_Record *rec,
    Vec3 *attenuation,
    Ray *scattered)
{
    _Bool res;
    switch (self.MATERIAL_KIND)
    {
    case KIND_METAL:
        res = metal_scatter(self.materials.metal, r_in, rec, attenuation, scattered);
        break;
    case KIND_LAMBERTIAN:
        res = lambertian_scatter(self.materials.lambertian, rec, attenuation, scattered);
        break;
    case KIND_DIELECTRIC:
        res = dielectric_scatter(self.materials.dielectric, r_in, rec, attenuation, scattered);
        break;
    default:
        assert(0 && "Invalid material\n");
    }

    return res;
}
