#include "ray.c"

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif

typedef struct {
	Vec3 lower_left_corner;
	Vec3 horizental;
	Vec3 vertical;
	Vec3 origin;
} Camera;

static Camera
camera_new(Vec3 look_from, Vec3 look_at, Vec3 v_up, float vfov, float aspect) {
	Camera self;

	float theta		  = vfov * M_PI / 180.0f;
	float half_height = tanf(theta / 2.0f);
	float half_width  = aspect * half_height;

	Vec3 u, v, w;
	w = vec3_unit(look_from - look_at);
	u = vec3_unit(vec3_cross(v_up, w));
	v = vec3_cross(w, u);

	self.origin			   = look_from;
	self.lower_left_corner = self.origin - half_width * u - half_height * v - w;
	self.horizental		   = 2 * half_width * u;
	self.vertical		   = 2 * half_height * v;

	return self;
}

static Ray
camera_get_ray(const Camera self, float u, float v) {
	return (Ray){self.origin, self.horizental * u + self.lower_left_corner + self.vertical * v - self.origin};
}
