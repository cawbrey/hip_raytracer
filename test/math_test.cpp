#include "../src/math.cpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

TEST( VectorTest, RandomUnitVectorLength ) {
    const FLOAT_T epsilon = 1e-6f;

    for ( int i = 0; i < 100; ++i ) {
        Vector v = random_unit_vector();
        EXPECT_NEAR( v.length(), 1.0f, epsilon ) << "Length is not close to 1 on iteration " << i;
    }
}
