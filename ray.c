#include "vec3.c"

typedef struct {
	Vec3 a;
	Vec3 b;
} Ray;

static Vec3
ray_point_at_parameter(Ray self, float t) {
	return self.a + t * self.b;
}
