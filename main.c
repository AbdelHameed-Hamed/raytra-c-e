#include "ray.c"
#include <stdio.h>

float hit_sphere(const Vec3 center, float radius, const Ray r) {
  Vec3 oc = vec3_sub_inplace(ray_origin(r), center);
  float a = vec3_dot(ray_direction(r), ray_direction(r));
  float b = 2.0 *  vec3_dot(oc, ray_direction(r));
  float c = vec3_dot(oc, oc) - radius * radius;
  float discriminant = b * b - 4 * a * c;
  return discriminant < 0 ? -1.0 : (-b - sqrt(discriminant)) / (2.0 * a);
}

Vec3 color(const Ray r) {
  float t = hit_sphere(vec3_from_floats((float[]){0.0, 0.0, -1.0}), 0.5, r);
  if (t > 0.0) {
    Vec3 N = vec3_make_unit(vec3_sub_inplace(ray_point_at_parameter(r, t), vec3_from_floats((float[]){0.0, 0.0, -1.0})));
    return vec3_mul(vec3_from_floats((float[]){ vec3_x(N) + 1, vec3_y(N) + 1, vec3_z(N) + 1}), 0.5);
  }

  Vec3 unit_direction = vec3_unit(ray_direction(r));
  t = 0.5 * vec3_y(unit_direction) + 1.0;
  return vec3_add(vec3_mul(vec3_from_floats((float[]){1.0, 1.0, 1.0}), (1.0 - t)),
                  vec3_mul(vec3_from_floats((float[]){0.5, 0.7, 1.0}), t       ));
}

int main() {
  FILE *fp;
  fp = fopen("image.ppm", "w");

  const unsigned short width = 1280, height = 720;
  fprintf(fp, "P3\n%d %d \n255\n", width, height);

  Vec3 lower_left_corner = vec3_from_floats((float[]){-2.0, -1.0, -1.0});
  Vec3 horizental        = vec3_from_floats((float[]){4.0, 0.0, 0.0});
  Vec3 vertical          = vec3_from_floats((float[]){0.0, 2.0, 0.0});
  Vec3 origin            = vec3_from_floats((float[]){0.0, 0.0, 0.0});

  for (short j = height - 1; j >= 0; --j)
    for (unsigned short i = 0; i < width; ++i) {
      float u = (float)i / (float)width;
      float v = (float)j / (float)height;

      Ray r = ray_set(ray_init(),
                      vec3_from_vec3(origin),
                      vec3_add_inplace(vec3_add_inplace(vec3_mul(horizental, u),
                                                        lower_left_corner),
                                       vec3_mul(vertical, v)));

      Vec3 col = color(r);

      unsigned char ir = (unsigned char)(255.99f * vec3_r(col));
      unsigned char ig = (unsigned char)(255.99f * vec3_g(col));
      unsigned char ib = (unsigned char)(255.99f * vec3_b(col));

      fprintf(fp, "%d %d %d\n", ir, ig, ib);

    }

  fclose(fp);

  return 0;
}
