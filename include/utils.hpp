#pragma once

#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

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

inline int edgeFunction(Point2D a, Point2D b, Point2D p) {
    return (p.x-a.x) * (b.y-a.y) - (p.y-a.y) * (b.x-a.x);
}

Projection fov_to_f(int height, float fov);

Point2D project(const glm::vec3& v, const int w, const int h, const float fov, float clippingPlane=0.01f);

Point2D project(const glm::vec3& v, const int w, const int h, const Projection& projection, float clippingPlane=0.01f);
