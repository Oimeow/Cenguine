#pragma once

#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "raylib.h"
#include <format>

// macos stores uint32_t in little endian format (reverses order basically)
// which means the value 0xAABBCCDD is stored in memory as 0xDDCCBBAA.
// equivalently, a value of 0xRRGGBBAA is stored in RAM as 0xAABBGGRR.
// raylib reads framebuffer as memory, not as representative of values,
// so raylib ends up interpreting the memory (abgr) as the specified (rgba).
// therefore we can either 
//  A) change the specified format to ABGR32 or
//  B) have a helper function inverse-endianize the value
// i32rgba is the answer to option B.
// constexpr makes the function provide no overhead.
constexpr uint32_t i32rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff) {
    return (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(g) << 8) | uint32_t(r);
}

// structs

struct Tri {
    uint32_t vs[3];

    // ctor
    Tri(uint32_t a, uint32_t b, uint32_t c) : vs{a,b,c} 
    {}

    uint32_t& operator[](size_t i) { return vs[i]; }
};


struct Point2D {
    int x; 
    int y;
    float z;
};


struct FaceVertex {
    uint32_t vertex = 0;
    uint32_t texcoord = 0;
    uint32_t normal = 0;
};


struct Transform3D {
    glm::vec3 pos;
    glm::quat rot;

    Transform3D(glm::vec3 pos, glm::quat rot) : pos(pos), rot(rot)
    {}
};

struct Projection {
    float f;
};

// helper functions

inline int signedParallelogramArea(Point2D a, Point2D b, Point2D p) {
    return (p.x-a.x) * (b.y-a.y) - (p.y-a.y) * (b.x-a.x);
}

Projection fov_to_f(int height, float fov);

Point2D project(const glm::vec3& v, const int w, const int h, const float fov, float clippingPlane=0.01f);

Point2D project(const glm::vec3& v, const int w, const int h, const Projection& projection, float clippingPlane=0.01f);

Color unpackColor(uint32_t color);

uint32_t packColor(Color color);

// debugging functions

std::string vecAsString(glm::vec3 v);

std::string quatAsString(glm::quat q);
