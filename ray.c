#include "vec3.c"

struct IRay {
	Vec3 a;
	Vec3 b;
};

typedef struct IRay Ray;

static Ray
ray_init() { return (Ray){ (Vec3){ 0.0f }, (Vec3){ 0.0f } }; }

static Ray
ray_set(Ray self, const Vec3 a, const Vec3 b) {
	memcpy(&self, &(Ray){a, b}, sizeof(Ray));

	return self;
}

static Vec3
ray_origin(const Ray self) {
	return self.a;
}

static Vec3
ray_direction(const Ray self) {
	return self.b;
}

static Vec3
ray_point_at_parameter(Ray self, float t) {
	return self.a + t * self.b;
}
