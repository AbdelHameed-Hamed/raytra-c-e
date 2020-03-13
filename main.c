#include "includes.c"
#include <stdio.h>

static Vec3
color(const Ray r, Sphere spheres[], int count) {
  IHit_Record rec, temp_rec;
  _Bool hit_anything   = 0.0f;
  float closest_so_far = __FLT_MAX__;
  
  for (int i = 0; i < count; ++i) {
    if (sphere_hit(spheres[0], r, 0.0f, __FLT_MAX__, &temp_rec)) {
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

int
main() {
  FILE *fp;
  fopen_s(&fp, "image.ppm", "w");

  const unsigned short WIDTH = 1280, HEIGHT = 720;
  fprintf(fp, "P3\n%d %d \n255\n", WIDTH, HEIGHT);

  Sphere sphere[2] = { 
      {(Vec3){0.0f,    0.0f, -1.0f},   0.5f},
      {(Vec3){0.0f, -100.5f, -1.0f}, 100.0f}
  };

  Camera cam = camera_new();

  for (short j = HEIGHT - 1; j >= 0; --j)
    for (unsigned short i = 0; i < WIDTH; ++i) {
      float u = (float)i / (float)WIDTH;
      float v = (float)j / (float)HEIGHT;

      Ray r = camera_get_ray(cam, u, v);

      Vec3 p = ray_point_at_parameter(r, 2.0f);
      Vec3 col = color(r, sphere, 2);

      unsigned char ir = (unsigned char)(255.99f * col.r);
      unsigned char ig = (unsigned char)(255.99f * col.g);
      unsigned char ib = (unsigned char)(255.99f * col.b);

      fprintf(fp, "%d %d %d\n", ir, ig, ib);
    }

  fclose(fp);

  return 0;
}
