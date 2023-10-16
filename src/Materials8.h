#pragma once

#include "Materials.h"

__vectorcall INLINE static void
lambertian8_scatter(
    const Vec3x8 albedo,
    const Hit_Records8 recs,
    Vec3x8 *attenuation,
    Rays8 *scattered)
{
    *scattered = (Rays8){
        recs.ps,
        vec3x8_add(recs.normals, random_unit_vec3x8())
    };
    *attenuation = albedo;
}

__vectorcall INLINE static Vec3x8
reflect8(const Vec3x8 vec, const Vec3x8 norm)
{
    return vec3x8_sub(vec, vec3x8_mul_f32(vec3x8_mul_f32x8(norm, vec3x8_dot(vec, norm)), 2));
}

INLINE static Vec3x8
randoms8_in_unit_sphere()
{
    f32x8 mask = {0};
    Vec3x8 ps = {0};
    while (!is_mask_saturated(&mask))
    {
        ps = vec3x8_blend(
            vec3x8_sub(
                vec3x8_mul_f32((Vec3x8){random_f32x8(), random_f32x8(), random_f32x8()}, 2),
                vec3x8_from_f32x8(ones)
            ),
            ps,
            mask
        );
        mask = _mm256_cmp_ps(vec3x8_mag_sqr(ps), ones, _CMP_LT_OQ);
    }

    return ps;
}

__vectorcall INLINE static f32x8
metal8_scatter(
    const Vec3x8 albedo,
    const f32x8 fuzz,
    const Hit_Records8 recs,
    const Rays8 rs_in,
    Vec3x8 *attenuation,
    Rays8 *scattered)
{
    const Vec3x8 reflected = reflect8(vec3x8_unit(rs_in.directions), recs.normals);
    *scattered = (Rays8){
        recs.ps,
        vec3x8_add(reflected, vec3x8_mul_f32x8(randoms8_in_unit_sphere(), fuzz))
    };
    *attenuation = albedo;

    return _mm256_cmp_ps(vec3x8_dot(scattered->directions, recs.normals), zeros, _CMP_GT_OQ);
}

__vectorcall INLINE static f32x8
refract8(const Vec3x8 vec, const Vec3x8 norm, const f32x8 ni_over_nt, Vec3x8* refracted)
{
    Vec3x8 uv = vec3x8_unit(vec);
    f32x8 dt = vec3x8_dot(uv, norm);
    f32x8 discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
    f32x8 mask = _mm256_cmp_ps(discriminant, zeros, _CMP_GT_OQ);
    {
        *refracted = vec3x8_blend(
            *refracted,
            vec3x8_sub(
                vec3x8_mul_f32x8(vec3x8_sub(uv, vec3x8_mul_f32x8(norm, dt)), ni_over_nt),
                vec3x8_mul_f32x8(norm, _mm256_sqrt_ps(discriminant))
            ),
            mask
        );
    }

    return mask;
}

__vectorcall INLINE static f32x8
schlick8(f32x8 cos, f32x8 ref_idx)
{
    f32x8 r_0 = (1 - ref_idx) / (1 + ref_idx);
    r_0 *= r_0;
    f32x8 one_minus_cos = 1.0f - cos;

    return r_0 + (1 - r_0) * (one_minus_cos * one_minus_cos * one_minus_cos * one_minus_cos * one_minus_cos);
}

__vectorcall INLINE static void
dielectric8_scatter(
    const f32x8 ref_idx,
    const Hit_Records8 recs,
    const Rays8 rs_in,
    Vec3x8 *attenuation,
    Rays8 *scattered)
{
    Vec3x8 outward_normal;
    Vec3x8 reflected = reflect8(rs_in.directions, recs.normals);
    f32x8 ni_over_nt;
    *attenuation = vec3x8_from_f32x8(ones);
    Vec3x8 refracted;
    f32x8 reflect_prob;
    f32x8 cos;

    f32x8 mask = _mm256_cmp_ps(vec3x8_dot(rs_in.directions, recs.normals), zeros, _CMP_GT_OQ);
    {
        outward_normal = vec3x8_blend(outward_normal, vec3x8_mul_f32(recs.normals, -1), mask);
        ni_over_nt = _mm256_blendv_ps(ni_over_nt, ref_idx, mask);
        cos = _mm256_blendv_ps(
            cos,
            ref_idx * vec3x8_dot(rs_in.directions, recs.normals) / vec3x8_mag(rs_in.directions),
            mask
        );
    }
    {
        outward_normal = vec3x8_blend(recs.normals, outward_normal, mask);
        ni_over_nt = _mm256_blendv_ps(1 / ref_idx, ni_over_nt, mask);
        cos = _mm256_blendv_ps(
            -vec3x8_dot(rs_in.directions, recs.normals) / vec3x8_mag(rs_in.directions),
            cos,
            mask
        );
    }

    mask = refract8(rs_in.directions, outward_normal, ni_over_nt, &refracted);
    {
        reflect_prob = _mm256_blendv_ps(ones, schlick8(cos, ref_idx), mask);
    }

    mask = _mm256_cmp_ps(random_f32x8(), reflect_prob, _CMP_LT_OQ);
    scattered->origins = recs.ps;
    scattered->directions = vec3x8_blend(refracted, reflected, mask);
}

__vectorcall INLINE static f32x8
materials8_scatter(
    const u32x8 indices,
    const Material materials[],
    const Rays8 rs_in,
    const Hit_Records8 recs,
    Vec3x8 *attenuation,
    Rays8 *scattered)
{
    f32x8
        res = {0},
        fuzz = {0},
        ref_idx = {0},
        metal_mask = {0},
        lambertian_mask = {0},
        dielectric_mask = {0};
    Vec3x8 albedo = {0}, temp_attenuation = {0};
    Rays8 temp_scattered = {0};

    const u32 saturated_u32 = 0xffffffffu;
    const f32 saturated_f32 = *(f32*)(&saturated_u32);

    for (int i = 0; i < 8; ++i)
    {
        Material material = materials[indices[i]];
        switch (material.MATERIAL_KIND)
        {
        case KIND_METAL: {
            fuzz[i] = material.materials.metal.fuzz;
            albedo.xs[i] = material.materials.metal.albedo.x;
            albedo.ys[i] = material.materials.metal.albedo.y;
            albedo.zs[i] = material.materials.metal.albedo.z;
            metal_mask[i] = saturated_f32;
        } break;
        case KIND_LAMBERTIAN: {
            albedo.xs[i] = material.materials.lambertian.albedo.x;
            albedo.ys[i] = material.materials.lambertian.albedo.y;
            albedo.zs[i] = material.materials.lambertian.albedo.z;
            lambertian_mask[i] = saturated_f32;
        } break;
        case KIND_DIELECTRIC: {
            ref_idx[i] = material.materials.dielectric.ref_idx;
            dielectric_mask[i] = saturated_f32;
        } break;
        default:
            assert(0 && "Invalid material\n");
        }
    }

    res = _mm256_and_ps(
        metal_mask,
        metal8_scatter(albedo, fuzz, recs, rs_in, &temp_attenuation, &temp_scattered)
    );
    *attenuation = vec3x8_blend(*attenuation, temp_attenuation, metal_mask);
    *scattered = (Rays8){
        vec3x8_blend(scattered->origins, temp_scattered.origins, metal_mask),
        vec3x8_blend(scattered->directions, temp_scattered.directions, metal_mask)
    };

    res = _mm256_or_ps(res, lambertian_mask);
    lambertian8_scatter(albedo, recs, &temp_attenuation, &temp_scattered);
    *attenuation = vec3x8_blend(*attenuation, temp_attenuation, lambertian_mask);
    *scattered = (Rays8){
        vec3x8_blend(scattered->origins, temp_scattered.origins, lambertian_mask),
        vec3x8_blend(scattered->directions, temp_scattered.directions, lambertian_mask)
    };

    res = _mm256_or_ps(res, dielectric_mask);
    dielectric8_scatter(ref_idx, recs, rs_in, &temp_attenuation, &temp_scattered);
    *attenuation = vec3x8_blend(*attenuation, temp_attenuation, dielectric_mask);
    *scattered = (Rays8){
        vec3x8_blend(scattered->origins, temp_scattered.origins, dielectric_mask),
        vec3x8_blend(scattered->directions, temp_scattered.directions, dielectric_mask)
    };

    return res;
}
