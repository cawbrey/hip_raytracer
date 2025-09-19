#ifndef SHITTYRAYTRACER_TEXTURE_H
#define SHITTYRAYTRACER_TEXTURE_H

#include "vec3.h"
#include "Image.h"

class Texture {
public:
    virtual ~Texture() = default;

    __device__ virtual Color value(float u, float v) const = 0;
};

class SolidColor : public Texture {
    Color color;
public:
    __device__ explicit SolidColor(Color c) : color(c) {}

    __device__ Color value(float u, float v) const override {
        return color;
    }
};

class CheckerBoardTexture : public Texture {
    float frequency;
    Color color1;
    Color color2;
public:
    __device__ explicit CheckerBoardTexture(Color c1, Color c2, float frequency) : frequency(frequency), color1(c1), color2(c2) {};

    __device__ Color value(float u, float v) const override {
        return ((uint) (v * frequency) + (uint) (u * frequency)) % 2 == 0 ? color1 : color2;
    }
};

class ImageTexture : public Texture {
    Image::Bitmap *bitmap;
public:
    __device__ explicit ImageTexture(Image::Bitmap *img) : bitmap(img) {};

    __device__ Color value(float u, float v) const override {
        uint y = uint(v * float(bitmap->height));
        uint x = uint(u * float(bitmap->width));
        return bitmap->getPixel(x, y);
    }
};

#endif //SHITTYRAYTRACER_TEXTURE_H
