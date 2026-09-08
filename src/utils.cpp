#include "utils.hpp"
#include <iostream>

Projection fov_to_f(int height, float fov) {
    return Projection(.5f*height / glm::tan(0.5f * glm::radians(fov)));
}

Point2D project(const glm::vec3& v, const int w, const int h, const float fov, float clippingPlane) {
    if (v.z <= clippingPlane) {
        return {0,0,v.z};
    }

    Projection f = fov_to_f(h, fov);
    int xf = int(0.5f * w + v.x * f.f / v.z);
    int yf = int(0.5f * h - v.y * f.f / v.z);

    return {xf, yf, v.z};
}

Point2D project(const glm::vec3& v, const int w, const int h, const Projection& projection, float clippingPlane) {
    if (v.z <= clippingPlane) {
        return {0,0,v.z};
    }

    int xf = int(0.5f * w + v.x * projection.f / v.z);
    int yf = int(0.5f * h - v.y * projection.f / v.z);

    return {xf, yf, v.z};
}

Color unpackColor(uint32_t color) {
    uint8_t r = color & 0xff;
    uint8_t g = (color >> 8) & 0xff;
    uint8_t b = (color >> 16) & 0xff;
    uint8_t a = color >> 24 & 0xff;

    return Color(r, g, b, a);
}

uint32_t packColor(Color color) {
    return i32rgba(color.r, color.g, color.b, color.a);
}