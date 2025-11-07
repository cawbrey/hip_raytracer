#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#define FLOAT_T float

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

struct Pose {
    glm::vec3 position;
    glm::quat rotation;
};

//
//
// inline auto rand_float( const FLOAT_T lower = -1.0F, const FLOAT_T upper = 1.0F ) -> FLOAT_T {
//     static std::random_device randomDevice;
//     static std::mt19937 gen( randomDevice() );
//     std::uniform_real_distribution dist( lower, upper );
//     return dist( gen );
// }
//
// inline auto random_vector( const FLOAT_T lower = -1.0F, const FLOAT_T upper = 1.0F ) -> Vector3 {
//     return Vector3{ rand_float( lower, upper ), rand_float( lower, upper ),
//                    rand_float( lower, upper ) };
// }
//
// inline auto random_unit_vector() -> Vector3 {
//     return random_vector().normalized();
// }