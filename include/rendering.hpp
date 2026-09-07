#pragma once

#include "display.hpp"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/trigonometric.hpp>
#include "math.h"
#include "components.hpp"
#include "utils.hpp"


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

void testShader(Display &d);

void vertexRender(Display &d, const std::vector<glm::vec3>& vs);

void wireframeRenderBFC(Display &d, Object& obj, std::vector<uint32_t>& visibleTris, const std::vector<glm::vec3>& wvs);

std::vector<uint32_t> cullBackFaces(Object& obj, Transform3D& tCamera, const std::vector<glm::vec3>& wvs);

std::vector<uint32_t> cullBackFacesScreen(Object& obj, const std::vector<Point2D>& projVs);

void rasterizeFill(Display &d, Object& obj, std::vector<uint32_t>& visibleTris, const std::vector<Point2D>& projVs);

void cullAndRasterize(Display &d, Object& obj, const std::vector<Point2D>& projVs);

#pragma region OLD

void wireframeRenderNaive(Display &d, Object& obj);

#pragma endregion
