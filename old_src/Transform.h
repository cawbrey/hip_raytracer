#ifndef SHITTYRAYTRACER_TRANSFORM_H
#define SHITTYRAYTRACER_TRANSFORM_H

#include "vec3.h"

class Transform {

    __device__ float degreesToRadians(float degrees) {
        return degrees * M_PI / 180;
    }


public:
    __device__ Transform(const point3 &position, const point3 &rotation, const point3 &scale) : t(position),
                                                                                  r(rotation),
                                                                                  s(scale) {


        r.y = degreesToRadians(r.y);
        r.x = degreesToRadians(r.x);
        r.z = degreesToRadians(r.z);
        updateMatrix();
    }

    __device__ Transform() : t(0, 0, 0), r(0, 0, 0), s(1, 1, 1) {
        r.y = degreesToRadians(r.y);
        r.x = degreesToRadians(r.x);
        r.z = degreesToRadians(r.z);
        updateMatrix();
    }

    __device__ void updateMatrix() {
        float translationMatrix[4][4] =
                {
                        {1, 0, 0, t.x},
                        {0, 1, 0, t.y},
                        {0, 0, 1, t.z},
                        {0, 0, 0, 1}
                };

        float rotationMatrixX[4][4] =
                {
                        {1, 0, 0, 0},
                        {0, cos(r.x), -sin(r.x), 0},
                        {0, sin(r.x), cos(r.x), 0},
                        {0, 0, 0, 1}
                };

        float rotationMatrixY[4][4] =
                {
                        {cos(r.y), 0, sin(r.y), 0},
                        {0, 1, 0, 0},
                        {-sin(r.y), 0, cos(r.y), 0},
                        {0, 0, 0, 1}
                };

        float rotationMatrixZ[4][4] =
                {
                        {cos(r.z), -sin(r.z), 0, 0},
                        {sin(r.z), cos(r.z), 0, 0},
                        {0, 0, 1, 0},
                        {0, 0, 0, 1}
                };

        float scaleMatrix[4][4] =
                {
                        {s.x, 0,   0,   0},
                        {0,   s.y, 0,   0},
                        {0,   0,   s.z, 0},
                        {0,   0,   0,   1}
                };

        float temp[4][4];
        multiply4x4Matrix(translationMatrix, rotationMatrixX, temp);
        multiply4x4Matrix(temp, rotationMatrixY, matrix);
        multiply4x4Matrix(matrix, rotationMatrixZ, temp);
        multiply4x4Matrix(temp, scaleMatrix, matrix);
    }

    __device__ inline void multiply4x4Matrix(float a[4][4], float b[4][4], float result[4][4]) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result[i][j] = 0;
                for (int k = 0; k < 4; k++) {
                    result[i][j] += a[i][k] * b[k][j];
                }
            }
        }
    }

    __device__ inline point3 applyToPoint(const point3 &p) const {
        // Multiply the point3 by the matrix
        float x = (p.x * matrix[0][0]) + (p.y * matrix[0][1]) + (p.z * matrix[0][2]) + matrix[0][3];
        float y = (p.x * matrix[1][0]) + (p.y * matrix[1][1]) + (p.z * matrix[1][2]) + matrix[1][3];
        float z = (p.x * matrix[2][0]) + (p.y * matrix[2][1]) + (p.z * matrix[2][2]) + matrix[2][3];
        return {x, y, z};
    }

private:
    point3 t;
    point3 r;
    point3 s;

    float matrix[4][4] =
            {
                    {1, 0, 0, 0},
                    {0, 1, 0, 0},
                    {0, 0, 1, 0},
                    {0, 0, 0, 1}
            };

};

#endif //SHITTYRAYTRACER_TRANSFORM_H
