#ifndef SHITTYRAYTRACER_SCENE_H
#define SHITTYRAYTRACER_SCENE_H

// #include "Hittable.h"
// #include "Texture.h"
// #include "Bvh.h"
// #include "Image.h"
// #include "camera.h"
// #include "sphere.h"
// #include "Material.h"
// #include "HipHelper.h"
// #include <thrust/host_vector.h>
// #include <thrust/device_vector.h>
// #include <thrust/device_ptr.h>
//
// namespace Scene{
//     //thrust::device_vector<Color*> h_images_data;
//     //thrust::device_vector<Image::Bitmap*> h_images;
//     static thrust::host_vector<Texture *> h_textures;
//     static thrust::host_vector<Material *> h_materials;
//     static thrust::host_vector<Hittable *> h_objects;
//     static Hittable* h_world;
//     static camera* h_camera;
//
////    //thrust::device_vector<Color*> d_images_data;
////    //thrust::device_vector<Image::Bitmap*> d_images;
////    static thrust::device_vector<Texture *> d_textures;
////    static thrust::device_vector<Material *> d_materials;
////    static thrust::device_vector<Hittable *> d_objects;
////    static thrust::device_ptr<Hittable> d_world;
////    static thrust::device_ptr<camera> d_camera;
//
//    ImageTexture *addImageTexture(Image::Bitmap *fileName) {
//        ImageTexture *texture;
//        checkHipErrors(hipMalloc((void **) &texture, sizeof(ImageTexture)));
//        checkHipErrors(hipMemcpy(texture, new ImageTexture(fileName),
//        sizeof(ImageTexture), hipMemcpyHostToDevice));
//        d_textures.push_back(texture);
//        return texture;
//    }
//
//    metal *
//
//    createMetal(const Color &a, double_t fuzz) {
//        metal *mat;
//        checkHipErrors(hipMalloc((void **) &mat, sizeof(metal)));
//        checkHipErrors(hipMemcpy(mat, new metal(a, fuzz), sizeof(metal),
//        hipMemcpyHostToDevice)); d_materials.push_back(mat); return mat;
//    }
//
//    dielectric *
//
//    createDielectric(float ri) {
//        dielectric *mat;
//        checkHipErrors(hipMalloc((void **) &mat, sizeof(dielectric)));
//        checkHipErrors(hipMemcpy(mat, new dielectric(ri), sizeof(dielectric),
//        hipMemcpyHostToDevice)); d_materials.push_back(mat); return mat;
//    }
//
//    static diffuse_light *
//
//    createDiffuseLight(const Color &a) {
//        diffuse_light *mat;
//        checkHipErrors(hipMalloc((void **) &mat, sizeof(diffuse_light)));
//        checkHipErrors(hipMemcpy(mat, new diffuse_light(a),
//        sizeof(diffuse_light), hipMemcpyHostToDevice));
//        d_materials.push_back(mat);
//        return mat;
//    }
//
//    lambertion *createLambertion(const Color &a) {
//        lambertion *mat;
//        checkHipErrors(hipMalloc((void **) &mat, sizeof(lambertion)));
//        checkHipErrors(hipMemcpy(mat, new lambertion(a), sizeof(lambertion),
//        hipMemcpyHostToDevice)); d_materials.push_back(mat); return mat;
//    }
//
//    sphere *createSphere(vec3 cen, float r, Material *m) {
//        // Allocate memory for the sphere object on the device
//        sphere *d_object;
//        checkHipErrors(hipMalloc((void **) &d_object, sizeof(sphere)));
//
//        // Copy the host sphere object to the device
//        checkHipErrors(hipMemcpy(d_object, new sphere(cen, r, m),
//        sizeof(sphere), hipMemcpyHostToDevice));
//
//        d_objects.push_back(d_object);
//
//        return d_object;
//    }
//
//    camera *setCamera(camera *h_camera) {
//        // Allocate memory on the device for the camera object
//        checkHipErrors(hipMalloc((void **) &d_camera, sizeof(camera)));
//
//        // Copy the host camera object to the device
//        checkHipErrors(hipMemcpy(&d_camera, h_camera, sizeof(camera),
//        hipMemcpyHostToDevice));
//
//        // Return the device pointer
//        return d_camera.get();
//    }
//
//    void createTestScene() {
//        // Create the sun
//        createSphere({-4, 1, 0}, 1, createDiffuseLight({1.0, 0.98, 0.96}));
//
//        // Set the camera
//        Scene::setCamera(new camera({13, 2, 3}, {0, 0, 0}, {0, 1, 0}, 30.0,
//        float(1920) / float(1080), 0.1, 10.0));
//    }
//};

#endif  // SHITTYRAYTRACER_SCENE_H