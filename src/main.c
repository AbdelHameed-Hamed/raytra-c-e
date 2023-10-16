#include <stdio.h>
#include <time.h>

#include "Sphere.h"
#include "Camera.h"
#include "Spheres8.h"
#include "Materials8.h"
#include "Compute_Group.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image-2.26\stb_image_write.h"

typedef struct {
    Sphere *spheres;
    int count;
} Sphere_World;

static Sphere_World
random_scene()
{
    Sphere *spheres = (Sphere *)malloc(sizeof(Sphere) * 500);
    spheres[0]      = (Sphere){
        .material = {
            .MATERIAL_KIND               = KIND_LAMBERTIAN,
            .materials.lambertian.albedo = {0.5f, 0.5f, 0.5f}
        },
        .center = (Vec3){0.0f, -1000.0f, 0.0f}, .radius = 1000.0f
    };

    int i = 1;
    for (int a = -11; a < 11; ++a)
    {
        for (int b = -11; b < 11; ++b)
        {
            float choose_mat = random_float();
            Vec3 center = {a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float()};

            if (vec3_mag(center - (Vec3){4.0f, 0.2f, 0.0f}) > 0.9f)
            {
                if (choose_mat < 0.8f)
                {
                    spheres[i++] = (Sphere){
                        .material = {
                            .MATERIAL_KIND               = KIND_LAMBERTIAN,
                            .materials.lambertian.albedo = {
                                random_float() * random_float(),
                                random_float() * random_float(),
                                random_float() * random_float()
                            }
                        },
                        .center = center, .radius = 0.2f
                    };
                }
                else if (choose_mat < 0.95f)
                {
                    spheres[i++] = (Sphere){
                        .material = {
                            .MATERIAL_KIND   = KIND_METAL,
                            .materials.metal = {
                                .albedo = {
                                    0.5f * (1.0f + random_float()),
                                    0.5f * (1.0f + random_float()),
                                    0.5f * (1.0f + random_float()),
                                },
                                .fuzz = 0.5f * random_float()
                            }
                        },
                        .center = center, .radius = 0.2f
                    };
                }
                else
                {
                    spheres[i++] = (Sphere){
                        .material = {
                            .MATERIAL_KIND                = KIND_DIELECTRIC,
                            .materials.dielectric.ref_idx = 1.5f
                        },
                        .center = center, .radius = 0.2f
                    };
                }
            }
        }
    }

    spheres[i++] = (Sphere){
        .material = {
            .MATERIAL_KIND                = KIND_DIELECTRIC,
            .materials.dielectric.ref_idx = 1.5f,
        },
        .center = {0.0f, 1.0f, 0.0f}, .radius = 1.0f
    };
    spheres[i++] = (Sphere){
        .material = {
            .MATERIAL_KIND               = KIND_LAMBERTIAN,
            .materials.lambertian.albedo = {0.4f, 0.2f, 0.1f}
        },
        .center = {-4.0f, 1.0f, 0.0f}, .radius = 1.0f
    };
    spheres[i++] = (Sphere){
        .material = {
            .MATERIAL_KIND   = KIND_METAL,
            .materials.metal = {.albedo = {0.7f, 0.6f, 0.5f}, .fuzz = 0.0f}
        },
        .center = {4.0f, 1.0f, 0.0f}, .radius = 1.0f
    };

    return (Sphere_World){.spheres = spheres, .count = i};
}

__vectorcall INLINE static Vec3
color(Ray r, Sphere spheres[], int count)
{
    Vec3 ray_color = {1.0f, 1.0f, 1.0f};

    for (u8 depth = 0; depth < 50; ++depth)
    {
        Hit_Record rec;
        _Bool hit_anything   = 0;
        float closest_so_far = __FLT_MAX__;
        Material sp_material = {0};

        for (int i = 0; i < count; ++i)
        {
            if (sphere_hit(spheres[i], r, 0.001f, closest_so_far, &rec))
            {
                hit_anything   = 1;
                closest_so_far = rec.t;
                sp_material    = spheres[i].material;
            }
        }

        if (hit_anything)
        {
            Ray scattered    = {0};
            Vec3 attenuation = {0};
            if (material_scatter(sp_material, r, &rec, &attenuation, &scattered))
            {
                r          = scattered;
                ray_color *= attenuation;
            }
            else
            {
                return (Vec3){0};
            }
        }
        else
        {
            Vec3 unit_direction = vec3_unit(r.direction);
            float t = 0.5f * unit_direction.y + 1.0f;
            return ray_color * ((1.0f - t) * (Vec3){1.0f, 1.0f, 1.0f} + t * (Vec3){0.5f, 0.7f, 1.0f});
        }
    }

    return ray_color;
}

typedef struct {
    Seperate_Sphere *spheres;
    Material *materials;
    size_t count;
} Seperate_Sphere_World;

static Seperate_Sphere_World
random_seperate_scene()
{
    Seperate_Sphere *spheres = (Seperate_Sphere *)malloc(sizeof(Seperate_Sphere) * 500);
    Material *materials = (Material *)malloc(sizeof(Material) * 500);
    spheres[0] = (Seperate_Sphere){ .center = (Vec3){0.0f, -1000.0f, 0.0f}, .radius = 1000.0f };
    materials[0] = (Material) {
        .MATERIAL_KIND = KIND_LAMBERTIAN,
        .materials.lambertian.albedo = {0.5f, 0.5f, 0.5f}
    };

    int i = 1;
    for (int a = -11; a < 11; ++a)
    {
        for (int b = -11; b < 11; ++b)
        {
            float choose_mat = random_float();
            Vec3 center = {a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float()};

            if (vec3_mag(center - (Vec3){4.0f, 0.2f, 0.0f}) > 0.9f)
            {
                if (choose_mat < 0.8f)
                {
                    spheres[i] = (Seperate_Sphere){ .center = center, .radius = 0.2f };
                    materials[i++] = (Material){
                        .MATERIAL_KIND               = KIND_LAMBERTIAN,
                        .materials.lambertian.albedo = {
                            random_float() * random_float(),
                            random_float() * random_float(),
                            random_float() * random_float()
                        }
                    };
                }
                else if (choose_mat < 0.95f)
                {
                    spheres[i] = (Seperate_Sphere){ .center = center, .radius = 0.2f };
                    materials[i++] = (Material){
                        .MATERIAL_KIND   = KIND_METAL,
                        .materials.metal = {
                            .albedo = {
                                0.5f * (1.0f + random_float()),
                                0.5f * (1.0f + random_float()),
                                0.5f * (1.0f + random_float()),
                            },
                            .fuzz = 0.5f * random_float()
                        }
                    };
                }
                else
                {
                    spheres[i] = (Seperate_Sphere){ .center = center, .radius = 0.2f };
                    materials[i++] = (Material){
                        .MATERIAL_KIND                = KIND_DIELECTRIC,
                        .materials.dielectric.ref_idx = 1.5f
                    };
                }
            }
        }
    }

    spheres[i] = (Seperate_Sphere){ .center = {0.0f, 1.0f, 0.0f}, .radius = 1.0f };
    materials[i++] = (Material){
        .MATERIAL_KIND                = KIND_DIELECTRIC,
        .materials.dielectric.ref_idx = 1.5f,
    };
    spheres[i] = (Seperate_Sphere){ .center = {-4.0f, 1.0f, 0.0f}, .radius = 1.0f };
    materials[i++] = (Material){
        .MATERIAL_KIND               = KIND_LAMBERTIAN,
        .materials.lambertian.albedo = {0.4f, 0.2f, 0.1f}
    };
    spheres[i] = (Seperate_Sphere){ .center = {4.0f, 1.0f, 0.0f}, .radius = 1.0f };
    materials[i++] = (Material){
        .MATERIAL_KIND   = KIND_METAL,
        .materials.metal = {.albedo = {0.7f, 0.6f, 0.5f}, .fuzz = 0.0f}
    };

    return (Seperate_Sphere_World){.spheres = spheres, .materials = materials, .count = i};
}

__vectorcall INLINE Vec3x8
colors8(Rays8 rs, const Seperate_Sphere_World world)
{
    f32x8 rays_done_mask = {0};
    const f32x8 t_min = _mm256_set1_ps(0.001f);

    const Vec3x8 zero_color = vec3x8_from_vec3((Vec3){0});
    Vec3x8 ray_colors = vec3x8_from_vec3((Vec3){1.0f, 1.0f, 1.0f});

    for (u8 depth = 0; (depth < 50) && !is_mask_saturated(&rays_done_mask); ++depth)
    {
        Hit_Records8 recs = {0};
        f32x8 hit_anything = {0};
        f32x8 closest_so_far = _mm256_set1_ps(__FLT_MAX__);
        u32x8 material_indices = {0};

        for (int i = 0; i < world.count; ++i)
        {
            const f32x8 hits_mask = _mm256_andnot_ps(
                rays_done_mask,
                sphere_hit_rays8(world.spheres[i], rs, t_min, closest_so_far, &recs)
            );
            {
                hit_anything = _mm256_or_ps(hit_anything, hits_mask);
                closest_so_far = _mm256_blendv_ps(closest_so_far, recs.ts, hits_mask);
                material_indices = _mm256_blendv_ps(
                    _mm256_castsi256_ps(material_indices),
                    _mm256_castsi256_ps(_mm256_set1_epi32(i)),
                    hits_mask
                );
            }
        }

        {
            Rays8 scattered = {0};
            Vec3x8 attenuation = {0};

            f32x8 scatter_mask = materials8_scatter(
                material_indices,
                world.materials,
                rs,
                recs,
                &attenuation,
                &scattered
            );
            f32x8 hit_and_scatter_mask = _mm256_and_ps(hit_anything, scatter_mask);
            {
                rs = (Rays8){
                    vec3x8_blend(rs.origins, scattered.origins, hit_and_scatter_mask),
                    vec3x8_blend(rs.directions, scattered.directions, hit_and_scatter_mask),
                };
                ray_colors = vec3x8_blend(ray_colors, vec3x8_mul_vec3x8(ray_colors, attenuation), hit_and_scatter_mask);
            }
            f32x8 hit_and_not_scatter_mask = _mm256_xor_ps(hit_anything, hit_and_scatter_mask);
            {
                ray_colors = vec3x8_blend(ray_colors, zero_color, hit_and_not_scatter_mask);
                rays_done_mask = _mm256_blendv_ps(rays_done_mask, full_mask, hit_and_not_scatter_mask);
            }
        }
        {
            Vec3x8 unit_directions = vec3x8_unit(rs.directions);
            f32x8 t = 0.5f * unit_directions.ys + 1.0f;
            ray_colors = vec3x8_blend(
                vec3x8_mul_vec3x8(
                    vec3x8_add(
                        vec3x8_mul_f32x8(vec3x8_from_f32x8(ones), 1 - t),
                        vec3x8_mul_f32x8(vec3x8_from_vec3((Vec3){0.5f, 0.7f, 1.0f}), t)
                    ),
                    ray_colors
                ),
                ray_colors,
                _mm256_or_ps(hit_anything, rays_done_mask)
            );
            rays_done_mask = _mm256_blendv_ps(full_mask, rays_done_mask, hit_anything);
        }
    }

    return ray_colors;
}

int
main()
{
    full_mask = _mm256_cmp_ps(ones, ones, _CMP_EQ_OQ);
    const u16 WIDTH = 640, HEIGHT = 360, SAMPLES = 64;

    Camera cam = camera_new(
        (Vec3){13.0f, 2.0f, 3.0f},
        (Vec3){ 0.0f, 0.0f, 0.0f},
        (Vec3){ 0.0f, 1.0f, 0.0f},
        20,
        (float)WIDTH / (float)HEIGHT,
        0.1f,
        10.0f
    );

    // Sphere_World world = random_scene();
    // u8 *image          = malloc(sizeof(u8) * WIDTH * HEIGHT * 3);

    // time_t start = clock();

    // // #pragma omp parallel for
    // for (u16 j = 0; j < HEIGHT; ++j)
    // {
    //     for (u16 i = 0; i < WIDTH; ++i)
    //     {
    //         Vec3 col = {0.0f};
    //         for (u16 k = 0; k < SAMPLES; ++k)
    //         {
    //             float u = (float)(i + random_float()) / (float)WIDTH;
    //             float v = (float)(j + random_float()) / (float)HEIGHT;

    //             Ray r = camera_get_ray(cam, u, v);
    //             col  += color(r, world.spheres, world.count);
    //         }
    //         col /= (float)SAMPLES;
    //         col  = (Vec3){sqrtf(col.x), sqrtf(col.y), sqrtf(col.z)};

    //         image[(WIDTH * j + i) * 3]     = (u8)(255 * col.r);
    //         image[(WIDTH * j + i) * 3 + 1] = (u8)(255 * col.g);
    //         image[(WIDTH * j + i) * 3 + 2] = (u8)(255 * col.b);
    //     }
    // }

    // time_t end = clock();

    Seperate_Sphere_World world = random_seperate_scene();
    u8 *image = malloc(sizeof(u8) * WIDTH * HEIGHT * 3);

    time_t start = clock();

    // #pragma omp parallel for schedule(dynamic)
    // for (u16 j = 0; j < HEIGHT; ++j)
    // {
    //     for (u16 i = 0; i < WIDTH; ++i)
    //     {
    //         Vec3x8 cols = {0};
    //         for (u16 k = 0; k < SAMPLES; k += 8)
    //         {
    //             f32x8 us = (i + random_f32x8()) / WIDTH;
    //             f32x8 vs = (j + random_f32x8()) / HEIGHT;

    //             Rays8 rs = camera_get_rays8(cam, us, vs);
    //             cols = vec3x8_add(colors8(rs, world), cols);
    //         }

    //         Vec3 col = {0};
    //         for (int i = 0; i < 8; ++i)
    //             col += (Vec3){cols.xs[i], cols.ys[i], cols.zs[i]};
    //         col /= (float)SAMPLES;
    //         col = (Vec3){sqrtf(col.x), sqrtf(col.y), sqrtf(col.z)};

    //         image[(WIDTH * j + i) * 3]     = (u8)(255 * col.r);
    //         image[(WIDTH * j + i) * 3 + 1] = (u8)(255 * col.g);
    //         image[(WIDTH * j + i) * 3 + 2] = (u8)(255 * col.b);
    //     }
    // }

    const u16 tile_width_height = 4;
    #pragma omp parallel for schedule(dynamic)
    for (u16 j = 0; j < HEIGHT; j += tile_width_height)
    {
        for (u16 i = 0; i < WIDTH; i += tile_width_height)
        {
            for (u16 tile_j = j; tile_j < j + tile_width_height; ++tile_j)
            {
                for (u16 tile_i = i; tile_i < i + tile_width_height; ++tile_i)
                {
                    Vec3x8 cols = {0};
                    for (u16 k = 0; k < SAMPLES; k += 8)
                    {
                        f32x8 us = (tile_i + random_f32x8()) / WIDTH;
                        f32x8 vs = (tile_j + random_f32x8()) / HEIGHT;

                        Rays8 rs = camera_get_rays8(cam, us, vs);
                        cols = vec3x8_add(colors8(rs, world), cols);
                    }

                    Vec3 col = {0};
                    for (u16 lane = 0; lane < 8; ++lane)
                        col += (Vec3){cols.xs[lane], cols.ys[lane], cols.zs[lane]};
                    col /= (float)SAMPLES;
                    col = (Vec3){sqrtf(col.x), sqrtf(col.y), sqrtf(col.z)};

                    image[(WIDTH * tile_j + tile_i) * 3]     = (u8)(255 * col.r);
                    image[(WIDTH * tile_j + tile_i) * 3 + 1] = (u8)(255 * col.g);
                    image[(WIDTH * tile_j + tile_i) * 3 + 2] = (u8)(255 * col.b);
                }
            }
        }
    }

    time_t end = clock();

    double milliseconds = difftime(end, start);

    printf("%lf\n", milliseconds / 1000);

    stbi_flip_vertically_on_write(1);
    stbi_write_jpg("image.jpg", WIDTH, HEIGHT, 3, image, 0);

    free(world.spheres);
    free(image);

    return 0;
}


