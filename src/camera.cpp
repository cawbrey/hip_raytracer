#pragma once
#include "math.cpp"

class Camera {
    const FLOAT_T _half_h, _half_v;

public:
    const Pose pose;
    const FLOAT_T h_fov, v_fov;

    Camera(const Pose &pose,
        const FLOAT_T h_fov,
        const FLOAT_T v_fov) : pose(pose),
                               h_fov(h_fov),
                               v_fov(v_fov),
                               _half_h(std::tan(h_fov * 0.5f)),
                               _half_v(std::tan(v_fov * 0.5f)) {
    }

    [[nodiscard]] auto get_ray(const FLOAT_T horizontal, const FLOAT_T vertical) const {
        const FLOAT_T x = (2.0F * horizontal - 1.0F) * _half_h;
        const FLOAT_T y = (1.0F - 2.0F * vertical) * _half_v;

        const Vector3 localDir = Vector3{x, y, -1.0F}.normalized();
        const Vector3 worldDir = pose.rotation.rotate(localDir);
        return Ray{.origin=pose.position, .direction=worldDir.normalized()};
    }
};