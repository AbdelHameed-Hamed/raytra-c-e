#include "includes.c"

#include <stdio.h>
#include <stdlib.h>

static Vec3
color(const Ray r, Sphere spheres[], int count) {
  IHit_Record rec, temp_rec;
  _Bool hit_anything   = 0.0f;
  float closest_so_far = __FLT_MAX__;
  
  for (int i = 0; i < count; ++i) {
    if (sphere_hit(spheres[i], r, 0.0f, closest_so_far, &temp_rec)) {
      hit_anything   = 1.0f;
      closest_so_far = temp_rec.t;
      rec            = temp_rec;
    }
  }

  if (hit_anything) {
    return 0.5f * (Vec3){rec.normal + 1};
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

  Sphere sphere[2] = { 
      {(Vec3){0.0f,    0.0f, -1.0f},   0.5f},
      {(Vec3){0.0f, -100.5f, -1.0f}, 100.0f}
  };

  Camera cam = camera_new();

  for (short j = HEIGHT - 1; j >= 0; --j)
    for (unsigned short i = 0; i < WIDTH; ++i) {
      Vec3 col = { 0.0f };
      for (unsigned short k = 0; k < SAMPLES; ++k) {
        float random_n = rand() / (float)RAND_MAX;
        random_n = random_n == 1.0f ? 0.0f : random_n;

        float u = (float)(i + random_n) / (float)WIDTH;
        float v = (float)(j + random_n) / (float)HEIGHT;
  
        Ray r = camera_get_ray(cam, u, v);
        Vec3 p = ray_point_at_parameter(r, 2.0f);
        col += color(r, sphere, 2);
      }
      col /= (float)SAMPLES;

      unsigned char ir = (unsigned char)(255.99f * col.r);
      unsigned char ig = (unsigned char)(255.99f * col.g);
      unsigned char ib = (unsigned char)(255.99f * col.b);

      fprintf(fp, "%d %d %d\n", ir, ig, ib);
    }

  fclose(fp);

  return 0;
}
