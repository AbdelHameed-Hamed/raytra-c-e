#include "ray.c"

typedef struct {
	Vec3 lower_left_corner;
	Vec3 horizental;
	Vec3 vertical;
	Vec3 origin;
} Camera;

static Camera
camera_new() {
	Camera self = {
		(Vec3){-2.0f, -1.0f, -1.0f},
		(Vec3){ 4.0f,  0.0f,  0.0f},
		(Vec3){ 0.0f,  2.0f,  0.0f},
		(Vec3){ 0.0f,  0.0f,  0.0f}
	};

	return self;
}

static Ray
camera_get_ray(const Camera self, float u, float v) {
	return (Ray){ self.origin, self.horizental * u + self.lower_left_corner + self.vertical * v };
}
