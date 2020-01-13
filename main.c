#include "ray.c"
#include <stdio.h>

Vec3 color(const Ray r) {
  Vec3 unit_direction = vec3_unit(ray_direction(r));
  float t = 0.5 * vec3_y(unit_direction) + 1.0;
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
      ray_destroy(r);

      unsigned char ir = (unsigned char)(255.99f * vec3_r(col));
      unsigned char ig = (unsigned char)(255.99f * vec3_g(col));
      unsigned char ib = (unsigned char)(255.99f * vec3_b(col));

      fprintf(fp, "%d %d %d\n", ir, ig, ib);

      vec3_destroy(col);
    }

  vec3_destroy(lower_left_corner);
  vec3_destroy(horizental);
  vec3_destroy(vertical);
  vec3_destroy(origin);

  fclose(fp);

  return 0;
}
