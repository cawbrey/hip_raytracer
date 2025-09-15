#ifndef SHITTYRAYTRACER_BVH_H
#define SHITTYRAYTRACER_BVH_H

#include "Hittable.h"
#include "BoundingBox.h"

class Bvh : public Hittable {
    Hittable *leftChild = nullptr, *rightChild = nullptr;
public:
    __device__ Bvh(Hittable **objects, int64_t start, int64_t end) {

        for (size_t object_index = start; object_index < end; object_index++)
            boundingBox = BoundingBox(boundingBox, objects[object_index]->boundingBox);

        int64_t object_span = end - start;
        if (end == start)
            return;

        if (object_span == 1) {
            leftChild = rightChild = objects[start];
        } else if (object_span == 2) {
            leftChild = objects[start];
            rightChild = objects[start + 1];
        } else {
            auto comparator = boundingBox.lengthX() > boundingBox.lengthY() ?
                              (boundingBox.lengthX() > boundingBox.lengthZ() ? box_x_compare : box_z_compare) :
                              (boundingBox.lengthY() > boundingBox.lengthZ() ? box_y_compare : box_z_compare);

            shittySorter(objects, start, end, comparator);

            int64_t mid = start + object_span / 2;
            leftChild = new Bvh(objects, start, mid);
            rightChild = new Bvh(objects, mid, end);
        }
    }

    __device__ bool hit(const ray3 &r, float tmin, float tmax, HitRecord &rec) const override{
        if (!boundingBox.hit(r, tmin, tmax))
            return false;

        bool hit_left = leftChild && leftChild->hit(r, tmin, tmax, rec);
        bool hit_right = rightChild && rightChild->hit(r, tmin, hit_left ? rec.t : tmax, rec);

        return hit_left || hit_right;
    }

private:
    __device__ static inline bool box_x_compare(const Hittable *a, const Hittable *b) {
        return a->boundingBox.minX() < b->boundingBox.minX();
    }

    __device__ static inline bool box_y_compare(const Hittable *a, const Hittable *b) {
        return a->boundingBox.minY() < b->boundingBox.minY();
    }

    __device__ static inline bool box_z_compare(const Hittable *a, const Hittable *b) {
        return a->boundingBox.minZ() < b->boundingBox.minZ();
    }

    __device__
    static void
    shittySorter(Hittable **objects, int64_t start, int64_t end, bool (*comparator)(const Hittable *a, const Hittable *b)) {
        // Loop over the range from start to end
        for (int64_t i = start; i < end - 1; i++) {
            // Find the minimum element in the unsorted portion
            int64_t min_index = i;
            for (int64_t j = i + 1; j < end; j++) {
                // Compare current object with the object at min_index using the comparator
                if (comparator(objects[j], objects[min_index])) {
                    min_index = j;
                }
            }

            // Swap the found minimum element with the element at index i
            if (min_index != i) {
                Hittable *temp = objects[i];
                objects[i] = objects[min_index];
                objects[min_index] = temp;
            }
        }
    }
};

#endif //SHITTYRAYTRACER_BVH_H
