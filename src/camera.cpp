#pragma once
#include "math.cpp"

class Camera {
    mat3 _direction_matrix;

public:
    Pose pose;

    Camera(const Pose& pose, const flt focal_length)
        : pose(pose)
    {
        _direction_matrix = mat3(
            pose.orientation * vec3(1, 0, 0),
            pose.orientation * vec3(0, 1, 0),
            pose.orientation * vec3(0, 0, -focal_length)
        );
    }

    Ray get_ray(const flt u, const flt v) const {
        return Ray(pose.position, _direction_matrix * vec3(u, v, -1));
    }
};