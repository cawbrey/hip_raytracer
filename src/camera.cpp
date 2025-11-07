#pragma once
#include "math.cpp"

class Camera {
    glm::mat3 _direction_matrix;

public:
    Pose pose;

    Camera(const Pose& pose_, const FLOAT_T focal_length)
        : pose(pose_)
    {
        glm::vec3 right   = pose.orientation * glm::vec3(1, 0, 0);
        glm::vec3 up      = pose.orientation * glm::vec3(0, 1, 0);
        glm::vec3 forward = pose.orientation * glm::vec3(0, 0, -focal_length);

        _direction_matrix = glm::mat3(
            right,
            up,
            forward
        );
    }

    Ray get_ray(FLOAT_T u, FLOAT_T v) const {
        return Ray(pose.position, _direction_matrix * glm::vec3(u, v, -1));
    }
};