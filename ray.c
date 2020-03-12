#include "vec3.c"

struct IRay {
	Vec3 A;
	Vec3 B;
};

typedef struct IRay Ray;

static Ray ray_init() { return (Ray){ vec3_init(), vec3_init() }; }

static Ray ray_set(Ray self, const Vec3 a, const Vec3 b) {
	memcpy(&self, &(Ray){a, b}, sizeof(Ray));

	return self;
}

static Vec3 ray_origin(const Ray self) { return self.A; }

static Vec3 ray_direction(const Ray self) { return self.B; }

static Vec3 ray_point_at_parameter(Ray self, float t) { return self.A + (self.B * t); }
