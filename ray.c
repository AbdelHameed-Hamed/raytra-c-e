#include "vec3.c"

typedef struct {
	Vec3 origin;
	Vec3 direction;
} Ray;

static Vec3
ray_point_at_parameter(const Ray self, float t) {
	return self.origin + t * self.direction;
}
