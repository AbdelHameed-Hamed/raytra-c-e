#include "sphere.c"

#include <assert.h>

#define hit(object, r, t_min, t_max, hit_rec) _Generic((object),							\
														Sphere:		   sphere_hit,			\
														Hittable_List: hittable_list_hit,	\
														default:	   deadend_hit			\
											  )((object), (r), (t_min), (t_max), (hit_rec))

typedef struct {
	Vec3 p;
	Vec3 normal;
	float t;
} IHit_Record;

typedef IHit_Record *Hit_Record;

static _Bool
sphere_hit(const Sphere s, const Ray r, float t_min, float t_max, Hit_Record rec) {
	Vec3 oc = r.origin - s.center;
	float a = vec3_mag_sqr(r.direction);
	float b = 2.0f *  vec3_dot(oc, r.direction);
	float c = vec3_mag_sqr(oc) - s.radius * s.radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0){ 
		float temp = (-b - sqrtf(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
            rec->t = temp;
            rec->p = ray_point_at_parameter(r, rec->t);
            rec->normal = (rec->p - s.center) / s.radius;
            return 1;
        }
        temp = (-b + sqrtf(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec->t = temp;
            rec->p = ray_point_at_parameter(r, rec->t);
            rec->normal = (rec->p - s.center) / s.radius;
            return 1;
        }
	}
	return -1;
}
