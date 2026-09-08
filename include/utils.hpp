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

struct vec3i {
    int x, y, z;

    vec3i(): x(0), y(0), z(0) {}
    vec3i(glm::vec3 v) : x(v.x), y(v.y), z(v.z) {}
    vec3i(int x, int y, int z) : x(x), y(y), z(z) {}

    static inline int dot(vec3i &a, vec3i &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    static inline float dot(vec3i &a, glm::vec3 &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;  // i think this will auto-cast to float? yes it will;
    }

    int& operator[](size_t i) { return (&x)[i]; }

    const int& operator[](size_t i) const { return (&x)[i]; }

    vec3i operator+(const vec3i& v) const { return { x + v.x, y + v.y, z + v.z }; }

    vec3i& operator+=(const vec3i& v) { 
        x += v.x;  y += v.y;  z += v.z;
        return *this;
    }
};

struct Tri {
    vec3i vs, uvs, vns;

    // ctor
    Tri(int a, int b, int c) : vs{a,b,c}, uvs{}, vns{}
    {}
    Tri(const vec3i &vs, const vec3i &uvs, const vec3i &vns) : vs(vs), uvs(uvs), vns(vns)
    {}
};


struct Point2D {
    int x, y;
    float z;  // for depth buffer
};


struct FaceVertex {
    uint32_t vertex = 0, texcoord = 0, normal = 0;
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

struct CColor {
    uint8_t r, g, b, a;

    CColor() {}
    CColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
    CColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(255) {}
    CColor(Color color) : r(color.r), g(color.g), b(color.b), a(color.a) {}

    CColor& shade(CColor &light, float &intensity) {
        r = std::clamp(this->r * (light.r / 255.0f) * intensity, 0.0f, 255.0f);
        g = std::clamp(this->g * (light.g / 255.0f) * intensity, 0.0f, 255.0f);
        b = std::clamp(this->b * (light.b / 255.0f) * intensity, 0.0f, 255.0f);
        
        return *this;
    }

    static CColor shade(CColor &material, CColor &light, float &intensity) {
        const uint8_t rShade = std::clamp(material.r * (light.r / 255.0f) * intensity, 0.0f, 255.0f);
        const uint8_t gShade = std::clamp(material.g * (light.g / 255.0f) * intensity, 0.0f, 255.0f);
        const uint8_t bShade = std::clamp(material.b * (light.b / 255.0f) * intensity, 0.0f, 255.0f);
        
        return {rShade, gShade, bShade, material.a};
    }
};

// helper functions

inline int signedParallelogramArea(Point2D a, Point2D b, Point2D p) {
    return (p.x-a.x) * (b.y-a.y) - (p.y-a.y) * (b.x-a.x);
}

Projection fov_to_f(int height, float fov);

Point2D project(const glm::vec3& v, const int w, const int h, const float fov, float clippingPlane=0.01f);

Point2D project(const glm::vec3& v, const int w, const int h, const Projection& projection, float clippingPlane=0.01f);

CColor intToColor(uint32_t color);

uint32_t colorToInt(CColor color);

// debugging functions

std::string vecAsString(const glm::vec3 &v);

std::string quatAsString(const glm::quat &q);

std::string colorAsString(const CColor &c);
