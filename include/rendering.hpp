#pragma once

#include "display.hpp"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/trigonometric.hpp>
#include "math.h"
#include "components.hpp"
#include "utils.hpp"


void testShader(Display &d);

void vertexRender(Display &d, const std::vector<glm::vec3>& vs);

void wireframeRenderBFC(Display &d, Object& obj, std::vector<uint32_t>& visibleTris);

std::vector<uint32_t> cullBackFaces(Object& obj, Transform3D& tCamera, const std::vector<glm::vec3>& wvs);

std::vector<uint32_t> cullBackFacesScreen(Object& obj, const std::vector<Point2D>& projVs);

void rasterizeFill(Display &d, Object& obj, std::vector<uint32_t>& visibleTris, const std::vector<Point2D>& projVs);

void backfaceCullZCullRasterizeLight(Display &d, Object& obj, const std::vector<Point2D>& projVs, const std::vector<Light*>& lights);

Color shade(Color &material, Color &light, float &intensity);

#pragma region OLD

void wireframeRenderNaive(Display &d, Object& obj);

#pragma endregion
