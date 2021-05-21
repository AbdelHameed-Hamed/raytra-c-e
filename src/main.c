#include <stdio.h>
#include <time.h>

#include "Sphere.h"
#include "Camera.h"
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

INLINE static Vec3
color(Ray r, Sphere spheres[], int count)
{
    TracyCZone(ctx, 1);
    defer{TracyCZoneEnd(ctx)};
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

// Note: Should probably optimize this further by looking for faster alternatives for sin, cos, and sqrtf
// might wanna check out this for sin and cos: https://www.embeddedrelated.com/showarticle/152.php, and
// this for sqrt: https://en.wikipedia.org/wiki/Newton%27s_method
int
main()
{
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

    Sphere_World world = random_scene();
    u8 *image          = malloc(sizeof(u8) * WIDTH * HEIGHT * 3);

    time_t start = clock();

    #pragma omp parallel for
    for (u16 j = 0; j < HEIGHT; ++j)
    {
        for (u16 i = 0; i < WIDTH; ++i)
        {
            Vec3 col = {0.0f};
            for (u16 k = 0; k < SAMPLES; ++k)
            {
                float u = (float)(i + random_float()) / (float)WIDTH;
                float v = (float)(j + random_float()) / (float)HEIGHT;

                Ray r = camera_get_ray(cam, u, v);
                col  += color(r, world.spheres, world.count);
            }
            col /= (float)SAMPLES;
            col  = (Vec3){sqrtf(col.x), sqrtf(col.y), sqrtf(col.z)};

            image[(WIDTH * j + i) * 3]     = (u8)(255 * col.r);
            image[(WIDTH * j + i) * 3 + 1] = (u8)(255 * col.g);
            image[(WIDTH * j + i) * 3 + 2] = (u8)(255 * col.b);
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
