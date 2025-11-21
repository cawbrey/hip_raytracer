#pragma once
// #define GLM_FORCE_ALIGNED_GENTYPES
// #define GLM_CONFIG_PRECISION_FLOAT
// #define GLM_FORCE_ALIGNED
// #define GLM_MESSAGES
// #define GLM_FORCE_ALIGNED
// #define GLM_FORCE_INLINE
// #define GLM_FUNC_QUALIFIER
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/random.hpp>
#include <iostream>
#include <random>


#define FLOAT_T float

using mat3 = glm::f64mat3;
using vec3 = glm::f64vec3;
using quat = glm::f64quat;
using flt = glm::float64;

struct Ray {
    vec3 origin;
    vec3 direction;

    Ray(auto origin, auto direction) :
        origin(origin),
        direction(glm::fastNormalize(direction)) {}
};

struct Pose {
    vec3 position;
    quat orientation;
};

inline vec3 cached_random_sample()
{
    static std::array<vec3, 1'000> samples;
    static std::once_flag init_flag;
    std::call_once(init_flag, [] {
        //for (auto &v : samples) v = glm::sphericalRand(0.7);
        for (auto &v : samples) v = glm::gaussRand(vec3(0.0), vec3(0.5));
    });

    // Round Robin - Global
    // static std::atomic<std::size_t> idx{0};
    // std::size_t i = idx.fetch_add(1, std::memory_order_relaxed) % samples.size();
    // return samples.at(i);

    // Round Robin - Local
    // static thread_local std::size_t idx{0};
    // idx = (idx + 1) % samples.size();
    // return samples.at(idx);

    // Random - Global
    thread_local std::mt19937 rng{std::random_device{}()};
    thread_local std::uniform_int_distribution<std::size_t> dist(0, samples.size() - 1);

    return samples.at(dist(rng));
}