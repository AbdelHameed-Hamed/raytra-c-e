#include "vec3.c"

struct IRay {
	Vec3 a;
	Vec3 b;
};

typedef struct IRay Ray;

static Vec3
ray_point_at_parameter(Ray self, float t) {
	return self.a + t * self.b;
}
