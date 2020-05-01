#include "includes.c"

#include <stdio.h>

static Vec3
color(const Ray r, Sphere spheres[], int count, int depth) {
  IHit_Record rec;
  _Bool hit_anything   = 0.0f;
  float closest_so_far = __FLT_MAX__;
  Material sp_material = {0};
  
  for (int i = 0; i < count; ++i) {
    if (sphere_hit(spheres[i], r, 0.001f, closest_so_far, &rec)) {
      hit_anything   = 1.0f;
      closest_so_far = rec.t;
      sp_material    = spheres[i].material;
    }
  }

  if (hit_anything) {
    Ray scattered    = {0};
    Vec3 attenuation = {0};
    if (depth < 50 && material_scatter(sp_material, r, &rec, &attenuation, &scattered)) {
      return attenuation * color(scattered, spheres, count, depth + 1);
    }
    else {
      return (Vec3){0.0f, 0.0f, 0.0f};
    }
  }
  else {
    Vec3 unit_direction = vec3_unit(r.direction);
    float t = 0.5f * unit_direction.y + 1.0f;
    return (1.0f - t) * (Vec3){1.0f, 1.0f, 1.0f} + t * (Vec3){0.5f, 0.7f, 1.0f};
  }
}


// clang -std=c18 -Wall -Weverything -Wextra -Wpedantic -Ofast main.c -o main.exe
int
main() {
  FILE *fp;
  fopen_s(&fp, "image.ppm", "w");

  const unsigned short WIDTH = 1280, HEIGHT = 720, SAMPLES = 100;
  fprintf(fp, "P3\n%d %d \n255\n", WIDTH, HEIGHT);

  #define SPHERES_NUM 5
  Sphere spheres[SPHERES_NUM] = {
      {{.MATERIAL_KIND = KIND_LAMBERTIAN, .materials.lambertian = {(Vec3){0.1f, 0.2f, 0.5f}}},
       (Vec3){ 0.0f,    0.0f, -1.0f},   0.5f},
      {{.MATERIAL_KIND = KIND_LAMBERTIAN, .materials.lambertian = {(Vec3){0.8f, 0.8f, 0.0f}}},
       (Vec3){ 0.0f, -100.5f, -1.0f}, 100.0f},
      {{.MATERIAL_KIND = KIND_METAL,      .materials.metal = {(Vec3){0.8f, 0.6f, 0.2f}, 0.0f}},
       (Vec3){ 1.0f,    0.0f, -1.0f},   0.5f},
      {{.MATERIAL_KIND = KIND_DIELECTRIC, .materials.dielectric = {1.5f}},
       (Vec3){-1.0f,    0.0f, -1.0f},   0.5f},
      {{.MATERIAL_KIND = KIND_DIELECTRIC, .materials.dielectric = {1.5f}},
       (Vec3){-1.0f,    0.0f, -1.0f}, -0.45f}
  };

  Camera cam = camera_new(
      (Vec3){-2.0f, 2.0f,  1.0f},
      (Vec3){ 0.0f, 0.0f, -1.0f},
      (Vec3){ 0.0f, 1.0f,  0.0f},
      90, (float)WIDTH / (float)HEIGHT
  );

  for (short j = HEIGHT - 1; j >= 0; --j)
    for (unsigned short i = 0; i < WIDTH; ++i) {
      Vec3 col = {0.0f};
      for (unsigned short k = 0; k < SAMPLES; ++k) {
        float u = (float)(i + random_float()) / (float)WIDTH;
        float v = (float)(j + random_float()) / (float)HEIGHT;
  
        Ray r = camera_get_ray(cam, u, v);
        col  += color(r, spheres, SPHERES_NUM, 0);
      }
      col /= (float)SAMPLES;
      col  = (Vec3){sqrtf(col.x), sqrtf(col.y), sqrtf(col.z)};

      unsigned char ir = (unsigned char)(255.99f * col.r);
      unsigned char ig = (unsigned char)(255.99f * col.g);
      unsigned char ib = (unsigned char)(255.99f * col.b);

      fprintf(fp, "%d %d %d\n", ir, ig, ib);
    }

  fclose(fp);

  return 0;
}
