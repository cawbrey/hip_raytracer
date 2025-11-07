#pragma once
#include <random>
#define FLOAT_T float

struct Vector3 {
    union {
        struct {
            FLOAT_T x1, x2, x3;
        };
        struct {
            FLOAT_T x, y, z;
        };
        struct {
            FLOAT_T r, g, b;
        };
    };

    auto operator+( const FLOAT_T input ) const {
        return Vector3{ x + input, y + input, z + input };
    }

    auto operator-( const FLOAT_T input ) const {
        return Vector3{ x - input, y - input, z - input };
    }

    auto operator*( const FLOAT_T input ) const {
        return Vector3{ x * input, y * input, z * input };
    }

    auto operator/( const FLOAT_T input ) const {
        return Vector3{ x / input, y / input, z / input };
    }

    auto operator-() const {
        return Vector3{ -x, -y, -z };
    }

    auto operator+( const Vector3& input ) const {
        return Vector3{ x + input.x, y + input.y, z + input.z };
    }

    auto operator-( const Vector3& input ) const {
        return Vector3{ x - input.x, y - input.y, z - input.z };
    }

    auto operator*( const Vector3& input ) const {
        return Vector3{ x * input.x, y * input.y, z * input.z };
    }

    auto operator/( const Vector3& input ) const {
        return Vector3{ x / input.x, y / input.y, z / input.z };
    }

    [[nodiscard]] auto dot( const Vector3& input ) const {
        return (x * input.x) + (y * input.y) + (z * input.z);
    }

    [[nodiscard]] auto cross( const Vector3& input ) const {
        return Vector3{ (y * input.z) - (z * input.y), (z * input.x) - (x * input.z), (x * input.y) - (y * input.x) };
    }

    [[nodiscard]] auto length() const {
        return std::sqrt( (x * x) + (y * y) + (z * z) );
    }

    [[nodiscard]] auto normalized() const {
        return *this / length();
    }
};

struct Quaternion {
    union {
        struct {
            FLOAT_T x, y, z, w;
        };
        struct {
            Vector3 v; // (x,y,z) part as a vector
            FLOAT_T s; // scalar part
        };
    };


    auto operator+(const Quaternion& input) const {
        return Quaternion{x + input.x, y + input.y, z + input.z, w + input.w};
    }

    auto operator-(const Quaternion& input) const {
        return Quaternion{x - input.x, y - input.y, z - input.z, w - input.w};
    }

    auto operator*(const FLOAT_T input) const {
        return Quaternion{x * input, y * input, z * input, w * input};
    }

    auto operator/(const FLOAT_T input) const {
        return Quaternion{x / input, y / input, z / input, w / input};
    }

    auto operator*(const Quaternion& input) const {
        return Quaternion{
            (w * input.x) + (x * input.w) + (y * input.z) - (z * input.y),
            (w * input.y) - (x * input.z) + (y * input.w) + (z * input.x),
            (w * input.z) + (x * input.y) - (y * input.x) + (z * input.w),
            (w * input.w) - (x * input.x) - (y * input.y) - (z * input.z)
        };
    }

    [[nodiscard]] auto conjugate() const {
        return Quaternion{-x, -y, -z, w};
    }

    [[nodiscard]] auto inverse() const {
        const FLOAT_T len2 = (x * x) + (y * y) + (z * z) + (w * w);
        return conjugate() / len2;
    }

    [[nodiscard]] auto rotate(const Vector3& v) const {
        Quaternion qv{v.x, v.y, v.z, 0.0f};
        Quaternion res = (*this) * qv * this->inverse();
        return Vector3{res.x, res.y, res.z};
    }
};

struct Ray {
    Vector3 origin;
    Vector3 direction;
};

struct Pose {
    Vector3 position;
    Quaternion rotation;
};


inline auto rand_float( const FLOAT_T lower = -1.0F, const FLOAT_T upper = 1.0F ) -> FLOAT_T {
    static std::random_device randomDevice;
    static std::mt19937 gen( randomDevice() );
    std::uniform_real_distribution dist( lower, upper );
    return dist( gen );
}

inline auto random_vector( const FLOAT_T lower = -1.0F, const FLOAT_T upper = 1.0F ) -> Vector3 {
    return Vector3{ rand_float( lower, upper ), rand_float( lower, upper ),
                   rand_float( lower, upper ) };
}

inline auto random_unit_vector() -> Vector3 {
    return random_vector().normalized();
}