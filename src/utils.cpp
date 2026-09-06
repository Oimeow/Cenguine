#include "utils.hpp"

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