#include "ray.c"
#include <stdio.h>

static float
hit_sphere(const Vec3 center, float radius, const Ray r) {
  Vec3 oc = r.a - center;
  float a = vec3_mag_sqr(r.b);
  float b = 2.0f *  vec3_dot(oc, r.b);
  float c = vec3_mag_sqr(oc) - radius * radius;
  float discriminant = b * b - 4 * a * c;
  return discriminant < 0 ? -1.0f : (-b - sqrtf(discriminant)) / (2.0f * a);
}

static Vec3
color(const Ray r) {
  float t = hit_sphere((Vec3){0.0f, 0.0f, -1.0f}, 0.5f, r);
  if (t > 0.0f) {
    Vec3 N = vec3_make_unit(ray_point_at_parameter(r, t) - (Vec3){0.0f, 0.0f, -1.0f});
    return (N + 1) * 0.5f;
  }

  Vec3 unit_direction = vec3_unit(r.b);
  t = 0.5f * unit_direction.y + 1.0f;
  return (Vec3){1.0f, 1.0f, 1.0f} * (1.0f - t) + (Vec3){0.5f, 0.7f, 1.0f} * t;
}

int
main() {
  FILE *fp;
  fopen_s(&fp, "image.ppm", "w");

  const unsigned short width = 1280, height = 720;
  fprintf(fp, "P3\n%d %d \n255\n", width, height);

  Vec3 lower_left_corner = (Vec3){-2.0f, -1.0f, -1.0f};
  Vec3 horizental        = (Vec3){ 4.0f,  0.0f,  0.0f};
  Vec3 vertical          = (Vec3){ 0.0f,  2.0f,  0.0f};
  Vec3 origin            = (Vec3){ 0.0f,  0.0f,  0.0f};

  for (short j = height - 1; j >= 0; --j)
    for (unsigned short i = 0; i < width; ++i) {
      float u = (float)i / (float)width;
      float v = (float)j / (float)height;

      Ray r = (Ray){ origin, horizental * u + lower_left_corner + vertical * v };

      Vec3 col = color(r);

      unsigned char ir = (unsigned char)(255.99f * col.r);
      unsigned char ig = (unsigned char)(255.99f * col.g);
      unsigned char ib = (unsigned char)(255.99f * col.b);

      fprintf(fp, "%d %d %d\n", ir, ig, ib);
    }

  fclose(fp);

  return 0;
}
