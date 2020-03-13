#include "sphere.c"

// Oughtta work on the default case. This feels like a dirty hack.
#define hit(object, r, t_min, t_max, hit_rec) _Generic((object),							\
														Sphere:		   sphere_hit,			\
														Hittable_List: hittable_list_hit,	\
														default:	   deadend_hit			\
											  )((object), (r), (t_min), (t_max), (hit_rec))

