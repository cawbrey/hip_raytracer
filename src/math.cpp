#include <cmath>
#include <random>
#define FLOAT_T float

struct Vector {
    union {
        struct {
            FLOAT_T x1, x2, x3;
        };
        struct {
            FLOAT_T x, y, z;
        };
    };

    auto operator+( const FLOAT_T input ) const {
        return Vector{ x + input, y + input, z + input };
    }

    auto operator-( const FLOAT_T input ) const {
        return Vector{ x - input, y - input, z - input };
    }

    auto operator*( const FLOAT_T input ) const {
        return Vector{ x * input, y * input, z * input };
    }

    auto operator/( const FLOAT_T input ) const {
        return Vector{ x / input, y / input, z / input };
    }

    auto operator-() const {
        return Vector{ -x, -y, -z };
    }

    auto operator+( const Vector& input ) const {
        return Vector{ x + input.x, y + input.y, z + input.z };
    }

    auto operator-( const Vector& input ) const {
        return Vector{ x - input.x, y - input.y, z - input.z };
    }

    auto operator*( const Vector& input ) const {
        return Vector{ x * input.x, y * input.y, z * input.z };
    }

    auto operator/( const Vector& input ) const {
        return Vector{ x / input.x, y / input.y, z / input.z };
    }

    [[nodiscard]] auto dot( const Vector& input ) const {
        return (x * input.x) + (y * input.y) + (z * input.z);
    }

    [[nodiscard]] auto cross( const Vector& input ) const {
        return Vector{ (y * input.z) - (z * input.y), (z * input.x) - (x * input.z), (x * input.y) - (y * input.x) };
    }

    [[nodiscard]] auto length() const {
        return std::sqrt( (x * x) + (y * y) + (z * z) );
    }

    [[nodiscard]] auto normalized() const {
        return *this / length();
    }
};

struct Ray {
    Vector origin;
    Vector direction;
};

auto rand_float( const FLOAT_T lower = -1.0F, const FLOAT_T upper = 1.0F ) -> FLOAT_T {
    static std::random_device randomDevice;
    static std::mt19937 gen( randomDevice() );
    std::uniform_real_distribution dist( lower, upper );
    return dist( gen );
}

auto random_vector( const FLOAT_T lower = -1.0F, const FLOAT_T upper = 1.0F ) -> Vector {
    return Vector{ rand_float( lower, upper ), rand_float( lower, upper ),
                   rand_float( lower, upper ) };
}

auto random_unit_vector() -> Vector {
    return random_vector().normalized();
}
