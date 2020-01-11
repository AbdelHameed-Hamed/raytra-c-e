#include "vec3.c"

struct IRay {
	struct IVec3 A;
	struct IVec3 B;
};

typedef struct IRay* Ray;

Ray ray_init() { return (Ray)malloc(sizeof(struct IRay)); }

void ray_destroy(Ray self) { free(self); }

Ray ray_set(Ray self, const Vec3 a, const Vec3 b) {
	vec3_set_vec3(&self->A, a);
	vec3_set_vec3(&self->B, b);

	return self;
}

const Vec3 ray_origin(const Ray self) { return &self->A; }

const Vec3 ray_direction(const Ray self) { return &self->B; }

const Vec3 ray_point_at_parameter(Ray self, float t) {
	return vec3_add(&self->A, vec3_mul(&self->B, t));
}
