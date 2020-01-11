#include "ray.c"
#include <stdio.h>

Vec3 color(const Ray r) {
  Vec3 unit_direction = vec3_unit(ray_direction(r));
  float t = 0.5 * vec3_y(unit_direction) + 1.0;
  return vec3_add(vec3_mul(vec3_from_floats(1.0, 1.0, 1.0), (1.0 - t)),
                  vec3_mul(vec3_from_floats(0.5, 0.7, 1.0), t       ));
}

int main() {
  FILE *fp;
  fopen_s(&fp, "image.ppm", "w");

  const unsigned char width = 200, height = 100;
  fprintf(fp, "P3\n%d %d \n255\n", width, height);

  Vec3 lower_left_corner = vec3_from_floats(-2.0, -1.0, -1.0);
  Vec3 horizental = vec3_from_floats(4.0, 0.0, 0.0);
  Vec3 vertical = vec3_from_floats(0.0, 2.0, 0.0);
  Vec3 origin = vec3_from_floats(0.0, 0.0, 0.0);

  for (short j = height - 1; j >= 0; --j)
    for (unsigned char i = 0; i < width; ++i) {
      float u = (float)i / (float)width;
      float v = (float)j / (float)height;

      Ray r = ray_set(ray_init(), origin, vec3_add(vec3_add(lower_left_corner,
                                                            vec3_mul(horizental, u)),
                                                   vec3_mul(vertical, v)));

      Vec3 col = color(r);

      unsigned char ir = (unsigned char)(255.99f * vec3_r(col));
      unsigned char ig = (unsigned char)(255.99f * vec3_g(col));
      unsigned char ib = (unsigned char)(255.99f * vec3_b(col));

      fprintf(fp, "%d %d %d\n", ir, ig, ib);

      vec3_destroy(col);
    }

  fclose(fp);

  return 0;
}
