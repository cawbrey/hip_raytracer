#ifndef SPHEREH
#define SPHEREH

#include "Hittable.h"

class sphere : public Hittable {
  __device__ static void uv(const point3 &p, float &u, float &v) {
        float phi = atan2(p.z, p.x) + M_PI;
        float theta = acos(-p.y);

        u = phi / (2 * M_PI);
        v = theta / M_PI;
    }

public:
    __device__ sphere(vec3 cen, float r, Material *m) : center(cen), radius(r) {
        mat_ptr = m;
        boundingBox = BoundingBox(center - vec3(radius, radius, radius),
                                  center + vec3(radius, radius, radius));
    };

  __device__ bool hit(const ray3 &r, float tmin, float tmax,
                      HitRecord &rec) const override {
        vec3 oc = r.origin - center;

        float a = dot(r.direction, r.direction);
        float b = dot(oc, r.direction);
        float c = dot(oc, oc) - radius * radius;

        float discriminant = b * b - a * c;

        if (discriminant > 0) {
            float temp = (-b - sqrt(discriminant)) / a;
            if (temp < tmax && temp > tmin) {
                rec.t = temp;
                rec.p = r(rec.t);
                rec.normal = (rec.p - center) / radius;
                rec.mat_ptr = mat_ptr;
                uv(rec.normal, rec.u, rec.v);
                return true;
            }
            temp = (-b + sqrt(discriminant)) / a;
            if (temp < tmax && temp > tmin) {
                rec.t = temp;
                rec.p = r(rec.t);
                rec.normal = (rec.p - center) / radius;
                rec.mat_ptr = mat_ptr;
                uv(rec.normal, rec.u, rec.v);
                return true;
            }
        }
        return false;
    }

    vec3 center;
    float radius;
};

//__device__ bool sphere::hit(const ray3 &r, float t_min, float t_max,
//hit_record &rec) const {
//
//    vec3 oc = r.origin - center;
//    float a = dot(r.direction, r.direction);
//    float b = dot(oc, r.direction);
//    float c = dot(oc, oc) - radius * radius;
//    float discriminant = b * b - a * c;
//    if (discriminant > 0) {
//        float temp = (-b - sqrt(discriminant)) / a;
//        if (temp < t_max && temp > t_min) {
//            rec.t = temp;
//            rec.p = r(rec.t);
//            rec.normal = (rec.p - center) / radius;
//            rec.mat_ptr = mat_ptr;
//            return true;
//        }
//        temp = (-b + sqrt(discriminant)) / a;
//        if (temp < t_max && temp > t_min) {
//            rec.t = temp;
//            rec.p = r(rec.t);
//            rec.normal = (rec.p - center) / radius;
//            rec.mat_ptr = mat_ptr;
//            return true;
//        }
//    }
//    return false;
//}

#endif