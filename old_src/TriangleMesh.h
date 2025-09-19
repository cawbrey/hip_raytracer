#ifndef SHITTYRAYTRACER_TRIANGLEMESH_H
#define SHITTYRAYTRACER_TRIANGLEMESH_H

#include <fstream>
#include <iostream>
#include <vector>

#include "Bvh.h"
#include "Hittable.h"
#include "Transform.h"
#include "Triangle.h"

void ReadObjFile(std::string &filename, std::vector<point3> faces) {
    std::vector<point3> vertices;
    std::ifstream objFile(filename);

    if (!objFile.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    std::string line;

    while (std::getline(objFile, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v" || token == "v") {
            double_t x, y, z;
            iss >> x >> y >> z;

            vertices.push_back({x, y, z});
        } else if (token == "f") {
            int a, b, c;
            iss >> a >> b >> c;

            faces.push_back(vertices[a - 1]);
            faces.push_back(vertices[b - 1]);
            faces.push_back(vertices[c - 1]);
        }
    }
}

class TriangleMesh : public Hittable {
    Hittable **faces;
    Bvh *meshHead;

public:
    ~TriangleMesh() {
        delete meshHead;

        for (int i = 0; i < sizeof(faces) / sizeof(faces[0]); i++) {
            delete faces[i];
        }

        delete[] faces;
    }

    __device__ TriangleMesh(point3 *vertices, uint numVertices,
                            Transform transform, Material *mat)
        : faces(), meshHead(nullptr) {
        mat_ptr = mat;
        faces = new Hittable *[numVertices / 3];

        for (int i = 0; i < numVertices; i += 3) {
            faces[i] =
                    new Triangle(vertices[i], vertices[i + 1], vertices[i + 2], mat);
        }

        // faces.push_back(std::make_shared<Triangle>(vertices[0], vertices[1],
        // vertices[2], mat));
        meshHead = new Bvh(faces, 0, numVertices / 3);

        boundingBox = meshHead->boundingBox;
    }

  __device__ virtual bool hit(const ray3 &r, float t_min, float t_max,
                              HitRecord &rec) const override {
        return meshHead->hit(r, t_min, t_max, rec);
    }
};

#endif  // SHITTYRAYTRACER_TRIANGLEMESH_H