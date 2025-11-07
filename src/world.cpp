#pragma once
#include <optional>
#include "math.cpp"

class Sphere {
    const FLOAT_T radius_squared;
public:
    const glm::u8vec3 color;
    const glm::vec3 center;
    const FLOAT_T radius;

    Sphere(
        const glm::vec3& center,
        const FLOAT_T radius,
        const glm::u8vec3& color)
        : color(color), center(center), radius(radius), radius_squared(radius * radius) {
    }

    [[nodiscard]] auto hit(const Ray& ray) const -> std::optional<FLOAT_T>{
        auto distance = 0.0F;
        bool hit = glm::intersectRaySphere(
            ray.origin,
            ray.direction,
            center,
            radius_squared,
            distance);

        if (!hit) {
            return std::nullopt;
        }

        return distance;
    }
};

class World {
public:
    const std::vector<Sphere> spheres;

    [[nodiscard]] auto cast_ray(const Ray &ray) const -> std::optional<std::pair<const Sphere *, FLOAT_T> > {
        FLOAT_T closestT = std::numeric_limits<FLOAT_T>::max();
        const Sphere *closestSphere = nullptr;

        for (const auto &sphere: spheres) {
            auto hitT = sphere.hit(ray);
            if (hitT.has_value() && hitT.value() < closestT) {
                closestT = hitT.value();
                closestSphere = &sphere;
            }
        }

        if (closestSphere != nullptr) {
            return std::make_pair(closestSphere, closestT);
        }

        return std::nullopt;
    }
};