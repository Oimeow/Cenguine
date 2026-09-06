#pragma once

#include <cstdint>

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