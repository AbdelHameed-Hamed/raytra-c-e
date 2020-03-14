#include "ray.c"

typedef struct {
	Vec3 lower_left_corner;
	Vec3 horizental;
	Vec3 vertical;
	Vec3 origin;
} Camera;

static Ray
camera_get_ray(const Camera self, float u, float v) {
	return (Ray){self.origin, self.horizental * u + self.lower_left_corner + self.vertical * v};
}
