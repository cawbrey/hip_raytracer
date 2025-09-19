#ifndef HITABLEH
#define HITABLEH

#include "BoundingBox.h"
#include "vec3.h"

class Material;

struct HitRecord {
    float t;
    vec3 p;
    vec3 normal;
    Material *mat_ptr;

    float u;
    float v;
};

class Hittable {
public:
    Material *mat_ptr{};
    BoundingBox boundingBox;

    __device__ virtual ~Hittable() = default;

  __device__ virtual bool hit(const ray3 &r, float t_min, float t_max,
                              HitRecord &rec) const = 0;
};

#endif