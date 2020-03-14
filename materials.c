#include "hittable.c"

#include <stdlib.h>
#include <assert.h>

static float
random_float() {
  float rand_n = rand() / (float)RAND_MAX;
  rand_n = rand_n == 1.0f ? 0.0f : rand_n;
  return rand_n;
}

static Vec3
random_in_unit_sphere() {
  Vec3 p = {0.0f};
  do {
    p = 2.0f * (Vec3){random_float(), random_float(), random_float()} - (Vec3){1.0f, 1.0f, 1.0f};
  } while (vec3_mag_sqr(p) >= 1.0f);
  return p;
}

typedef struct {
	Vec3 albedo;
} Lambertian;

static _Bool
lambertian_scatter(const Lambertian self,
				   const Hit_Record rec,
				   Vec3 *attenuation,
				   Ray *scattered) {
	Vec3 target = rec->normal + random_in_unit_sphere();
	*scattered = (Ray){rec->p, target};
	*attenuation = self.albedo;
	return 1;
}

typedef struct {
	Vec3 albedo;
	float fuzz;
} Metal;

static _Bool
Metal_scatter(const Metal self,
			  const Ray r_in,
			  const Hit_Record rec,
			  Vec3 *attenuation,
			  Ray *scattered) {
	Vec3 reflected = vec3_reflect(vec3_unit(r_in.direction), rec->normal);
	*scattered = (Ray){rec->p, reflected + self.fuzz * random_in_unit_sphere()};
	*attenuation = self.albedo;
	return (vec3_dot(scattered->direction, rec->normal) > 0);
}

typedef struct {
	enum {
		KIND_LAMBERTIAN,
		KIND_METAL
	} MATERIAL_KIND;

	union {
		Lambertian lambertian;
		Metal metal;
	} materials;
} Material;

static _Bool
material_scatter(Material self,
				 const Ray r_in,
				 const Hit_Record rec,
				 Vec3 *attenuation,
				 Ray *scattered) {
	_Bool res;
	switch (self.MATERIAL_KIND) {
		case KIND_LAMBERTIAN:
			res = lambertian_scatter(self.materials.lambertian, rec, attenuation, scattered);
			break;
		case KIND_METAL:
			res = Metal_scatter(self.materials.metal, r_in, rec, attenuation, scattered);
			break;
		default: assert(0 && "Invalid material\n");
	}

	return res;
}
