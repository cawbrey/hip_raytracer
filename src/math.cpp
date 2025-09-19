#include <cmath>
#include <random>
#define FLOAT_T float

typedef struct vector {
    union {
        struct {
            FLOAT_T x1, x2, x3;
        };
        struct {
            FLOAT_T x, y, z;
        };
        FLOAT_T data[3];
    };

    vector operator+( const FLOAT_T v ) const {
        return vector{ x + v, y + v, z + v };
    }

    vector operator-( const FLOAT_T v ) const {
        return vector{ x - v, y - v, z - v };
    }

    vector operator*( const FLOAT_T v ) const {
        return vector{ x * v, y * v, z * v };
    }

    vector operator/( const FLOAT_T v ) const {
        return vector{ x / v, y / v, z / v };
    }

    vector operator-() const {
        return vector{ -x, -y, -z };
    }

    vector operator+( const vector& v ) const {
        return vector{ x + v.x, y + v.y, z + v.z };
    }

    vector operator-( const vector& v ) const {
        return vector{ x - v.x, y - v.y, z - v.z };
    }

    vector operator*( const vector& v ) const {
        return vector{ x * v.x, y * v.y, z * v.z };
    }

    vector operator/( const vector& v ) const {
        return vector{ x / v.x, y / v.y, z / v.z };
    }

    [[nodiscard]] FLOAT_T dot( const vector& v ) const {
        return x * v.x + y * v.y + z * v.z;
    }

    [[nodiscard]] vector cross( const vector& v ) const {
        return vector{ y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
    }

    [[nodiscard]] FLOAT_T length() const {
        return std::sqrt( x * x + y * y + z * z );
    }

    [[nodiscard]] vector normalized() const {
        return *this / length();
    }
} vector_t;

typedef struct ray {
    vector origin;
    vector direction;
} ray_t;

FLOAT_T rand_float( const FLOAT_T lower = -1.0f, const FLOAT_T upper = 1.0f ) {
    static std::random_device rd;
    static std::mt19937 gen( rd() );
    std::uniform_real_distribution dist( lower, upper );
    return dist( gen );
}

vector random_vector( const FLOAT_T lower = -1.0f, const FLOAT_T upper = 1.0f ) {
    return vector{ rand_float( lower, upper ), rand_float( lower, upper ),
                   rand_float( lower, upper ) };
}

vector random_unit_vector() {
    return random_vector().normalized();
}
