#ifndef SHITTYRAYTRACER_BOUNDINGBOX_H
#define SHITTYRAYTRACER_BOUNDINGBOX_H

#include <hip/hip_vector_types.h>

#include "vec3.h"

class BoundingBox {
  __device__ static bool inline contains(float2 i, float t) {
        return i.x <= t && t <= i.y;
    }

    float2 x, y, z;

public:
    __device__ BoundingBox() : x(0, 0), y(0, 0), z(0, 0) {
    }

    __device__ BoundingBox(const float2 &x, const float2 &y, const float2 &z)
        : x(x), y(y), z(z) {
    }

    __device__ BoundingBox(const point3 &min, const point3 &max)
        : x(fmin(min.x, max.x), fmax(min.x, max.x)),
          y(fmin(min.y, max.y), fmax(min.y, max.y)),
          z(fmin(min.z, max.z), fmax(min.z, max.z)) {
    }

    __device__ BoundingBox(const BoundingBox &box1, const BoundingBox &box2) {
        x = {fmin(box1.x.x, box2.x.x), fmax(box1.x.y, box2.x.y)};
        y = {fmin(box1.y.x, box2.y.x), fmax(box1.y.y, box2.y.y)};
        z = {fmin(box1.z.x, box2.z.x), fmax(box1.z.y, box2.z.y)};
    }

  __device__ bool hit(const ray3 &r, float t_min, float t_max) const {
        float t1 = (x.x - r.origin.x) / r.direction.x;
        float t2 = (x.y - r.origin.x) / r.direction.x;

        float tmin = fmin(t1, t2);
        float tmax = fmax(t1, t2);

        t1 = (y.x - r.origin.y) / r.direction.y;
        t2 = (y.y - r.origin.y) / r.direction.y;

        tmin = fmax(tmin, fmin(t1, t2));
        tmax = fmin(tmax, fmax(t1, t2));

        t1 = (z.x - r.origin.z) / r.direction.z;
        t2 = (z.y - r.origin.z) / r.direction.z;

        tmin = fmax(tmin, fmin(t1, t2));
        tmax = fmin(tmax, fmax(t1, t2));

        return tmax > fmax(tmin, t_min) && tmin < t_max;
    }

  __device__ bool contains(const point3 &p) const {
        return contains(x, p.x) && contains(y, p.y) && contains(z, p.z);
    }

  __device__ float lengthX() const { return x.y - x.x; }

  __device__ float lengthY() const { return y.y - y.x; }

  __device__ float lengthZ() const { return z.y - z.x; }

  __device__ float minX() const { return x.x; }

  __device__ float minY() const { return y.x; }

  __device__ float minZ() const { return z.x; }

  __device__ float maxX() const { return x.y; }

  __device__ float maxY() const { return y.y; }

  __device__ float maxZ() const { return z.y; }
};

#endif  // SHITTYRAYTRACER_BOUNDINGBOX_H