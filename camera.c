#include "ray.c"

static Vec3 
random_vec3_in_unit_disk() {
	Vec3 res;
	do {
		res = 2.0f * (Vec3){random_float(), random_float(), 0.0f} - (Vec3){1.0f, 1.0f, 0.0f};
	} while (vec3_mag_sqr(res) >= 1.0f);
	return res;
}

typedef struct {
	Vec3 lower_left_corner;
	Vec3 horizental;
	Vec3 vertical;
	Vec3 origin;
	Vec3 u, v, w;
	float lens_radius;
} Camera;

static Camera
camera_new(Vec3 look_from,
		   Vec3 look_at,
		   Vec3 v_up,
		   float vfov,
		   float aspect,
		   float aperture,
		   float focus_dist) {
	Camera self;

	float theta		  = vfov * M_PI / 180.0f;
	float half_height = tanf(theta / 2.0f);
	float half_width  = aspect * half_height;
	self.lens_radius  = aperture / 2.0f;

	self.w = vec3_unit(look_from - look_at);
	self.u = vec3_unit(vec3_cross(v_up, self.w));
	self.v = vec3_cross(self.w, self.u);

	self.origin			   = look_from;
	self.lower_left_corner = self.origin -
							 half_width * focus_dist * self.u -
							 half_height * focus_dist * self.v -
							 focus_dist * self.w;
	self.horizental		   = 2 * half_width * focus_dist * self.u;
	self.vertical		   = 2 * half_height * focus_dist * self.v;

	return self;
}

static Ray
camera_get_ray(const Camera self, float u, float v) {
	Vec3 rd 	= self.lens_radius * random_vec3_in_unit_disk();
	Vec3 offset = self.u * rd.x + v * rd.y;
	return (Ray){self.origin + offset,
				 self.horizental * u + self.lower_left_corner + self.vertical * v - self.origin - offset};
}
