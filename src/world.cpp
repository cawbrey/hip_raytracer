#pragma once
#include <optional>
#include "math.cpp"

class Sphere {
public:
    const glm::u8vec3 color;
    const glm::vec3 center;
    const FLOAT_T radius;

    Sphere(
        const glm::vec3& center,
        const FLOAT_T radius,
        const glm::u8vec3& color)
        : color(color), center(center), radius(radius) {
    }

    [[nodiscard]] auto hit(const Ray& r, FLOAT_T t_min, FLOAT_T t_max) const -> std::optional<FLOAT_T> {
        // Ray-sphere intersection using quadratic formula
        glm::vec3 oc = r.origin - center;
        FLOAT_T a = dot(r.direction, r.direction);
        FLOAT_T b = 2.0f * dot(oc, r.direction);
        FLOAT_T c = dot(oc, oc) - (radius * radius);
        FLOAT_T discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return std::nullopt;
        }

        FLOAT_T sqrt_d = std::sqrt(discriminant);
        FLOAT_T t = (-b - sqrt_d) / (2.0f * a);

        if (t < t_min || t > t_max) {
            t = (-b + sqrt_d) / (2.0f * a);
            if (t < t_min || t > t_max) {
                return std::nullopt;
            }
        }

        return t;
    }
};

class World {
public:
    const std::vector<Sphere> spheres;

    [[nodiscard]] auto cast_ray(const Ray& ray) const -> std::optional<std::pair<const Sphere*, FLOAT_T>> {
        FLOAT_T closestT = std::numeric_limits<FLOAT_T>::max();
        const Sphere* closestSphere = nullptr;

        for (const auto& sphere : spheres) {
            auto hitT = sphere.hit(ray, 0.001F, closestT);
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