#pragma once
#include <optional>
#include "math.cpp"


class Material{
    const vec3 albedo, emittance;

public:
    Material(const vec3& albedo, const vec3& emittance) :
        albedo(albedo), emittance(emittance){
    }

    [[nodiscard]] auto luminance(
        const vec3& incident_luminance
        ) const {

        const auto reflected_luminance = (incident_luminance * albedo);

        const auto r = glm::max(reflected_luminance.r, emittance.r);
        const auto g = glm::max(reflected_luminance.g, emittance.g);
        const auto b = glm::max(reflected_luminance.b, emittance.b);

        return vec3(r, g, b);
    }

    [[nodiscard]] auto reflect(
        const Ray& incident_ray,
        const Ray& hit_normal
        ) const {

        auto reflected_direction = glm::reflect(
            incident_ray.direction,
            hit_normal.direction + cached_random_sample());

        return Ray(hit_normal.origin, reflected_direction);
    }
};

class Sphere {
public:
    const vec3 center;
    const flt radius;
    const Material material;

    Sphere(
        const vec3& center,
        const flt radius,
        const vec3& albedo,
        const vec3& emittance)
        : center(center), radius(radius), material(albedo, emittance) {
    }

    auto hit(const Ray& incident_ray) const -> std::optional<Ray>{
        auto point = vec3();
        auto normal = vec3();

        const bool hit = glm::intersectRaySphere(
            incident_ray.origin,
            incident_ray.direction,
            center,
            radius,
            point,
            normal);

        if (!hit) {
            return std::nullopt;
        }

        return Ray(point + (normal * 1e-4), normal);
    }
};

class World {
    [[nodiscard]] auto cast_ray(const Ray &ray) const -> std::optional<std::pair<const Sphere*, Ray>> {
        auto closestT = std::numeric_limits<glm::highp_f64>::max();
        Ray closestHitNormal = Ray(vec3(0.0f), vec3(0.0f, 0.0f, 1.0f));
        const Sphere *closestSphere = nullptr;


        for (const auto &sphere: spheres) {
            auto hitRay = sphere.hit(ray);
            if (hitRay.has_value()) {
                auto distance = glm::fastDistance(ray.origin, hitRay.value().origin);
                if (distance < closestT) {
                    closestT = distance;
                    closestHitNormal = hitRay.value();
                    closestSphere = &sphere;
                }
            }
        }

        if (closestSphere != nullptr) {
            return std::make_pair(closestSphere, closestHitNormal);
        }

        return std::nullopt;
    }
    
public:
    const std::vector<Sphere> spheres;

    [[nodiscard]] auto recursive_ray_trace(const Ray &incident_ray, const uint num_bounces) const {
        if (num_bounces == 0) {
            return vec3(0);
        }

        auto result = cast_ray(incident_ray);

        if (!result.has_value()) {
            return vec3(0);
        }

        auto resultant_ray = result.value().first->material.reflect(incident_ray, result.value().second);

        return result.value().first->material.luminance(recursive_ray_trace(resultant_ray, num_bounces - 1));
    }

    [[nodiscard]] auto trace_ray(const Ray &initial_ray, const int max_bounces = 50) const {
        return recursive_ray_trace(initial_ray, max_bounces);
    }
};