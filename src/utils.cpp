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

CColor intToColor(uint32_t color) {
    uint8_t r = color & 0xff;
    uint8_t g = (color >> 8) & 0xff;
    uint8_t b = (color >> 16) & 0xff;
    uint8_t a = color >> 24 & 0xff;

    return CColor(r, g, b, a);
}

uint32_t colorToInt(CColor color) {
    return i32rgba(color.r, color.g, color.b, color.a);
}

// debugging tools

std::string vecAsString(const glm::vec3 &v) {
    return std::format("vec3({}, {}, {})", v.x, v.y, v.z);
}

std::string quatAsString(const glm::quat &q) {
    return std::format("quat({}, {}, {}, {})", q.w, q.x, q.y, q.z);
}

std::string colorAsString(const CColor &c) {
    return std::format("CColor(r = {},  g = {},  b = {},  a = {})", c.r, c.g, c.b, c.a);
}
