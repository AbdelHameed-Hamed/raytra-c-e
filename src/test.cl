#include "Base.cl"

typedef struct {
    float3 origin;
    float3 direction;
} Ray;

float3
ray_point_at_parameter(const Ray self, const float t) {
    return self.origin + t * self.direction;
}

typedef struct {
    float3 p;
    float3 normal;
    float t;
} Hit_Record;

typedef struct {
    float3 center;
    float radius;
} Sphere;

kernel void
spheres_hit(constant Sphere *spheres, constant Ray *r_p, global Hit_Record *rec) {
    local Hit_Record recs[SPHERES_COUNT];
    recs[get_global_id(0)] = {0};

    const Ray r              = (*r_p);
    const Sphere sphere      = spheres[get_global_id(0)];
    const float3 oc          = r.origin - sphere.center;
    const float a            = dot(r.direction, r.direction);
    const float b            = dot(oc, r.direction);
    const float c            = dot(oc, oc) - sphere.radius * sphere.radius;
    const float discriminant = b * b - a * c;

    if (discriminant > 0) {
        const float temp_0 = (-b - sqrt(discriminant)) / a;
        const float temp_1 = (-b + sqrt(discriminant)) / a;

        if (temp_0 > 0) {
            recs[get_global_id(0)].t      = temp_0;
            recs[get_global_id(0)].p      = ray_point_at_parameter(r, recs[get_global_id(0)].t);
            recs[get_global_id(0)].normal = (recs[get_global_id(0)].p - sphere.center) / sphere.radius;
        } else if (temp_1 > 0) {
            recs[get_global_id(0)].t      = temp_1;
            recs[get_global_id(0)].p      = ray_point_at_parameter(r, recs[get_global_id(0)].t);
            recs[get_global_id(0)].normal = (recs[get_global_id(0)].p - sphere.center) / sphere.radius;
        }
    }

    work_group_barrier(CLK_LOCAL_MEM_FENCE);

    for (int i = get_global_size(0) / 2; i > 0; i /= 2) {
        int idx = i * 2;
        recs[idx] = (recs[idx].t > 0 && recs[idx].t < recs[idx + 1].t) ? recs[idx] : recs[idx + 1];
        work_group_barrier(CLK_LOCAL_MEM_FENCE);
    }

    *rec = recs[0];
}
