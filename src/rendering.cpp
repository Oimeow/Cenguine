#include "rendering.hpp"
#include <iostream>
#include "components.hpp"
#include "utils.hpp"
#include <algorithm>



void testShader(Display &d) {
    uint32_t width = d.W(), height = d.H();

    for (uint32_t y = 0; y < height; y++) {

        for (uint32_t x = 0; x < width; x++) {
            uint32_t r = 255*(x) / width;
            uint32_t g = 255*(height - y) / height;
            uint32_t b = 0;
        
            d.putPixel(x, y, i32rgba(r,g,b));
        }
    }
}

void vertexRender(Display &d, const std::vector<glm::vec3>& vs) {
    for (int i = 0; i < vs.size(); i++) {
        Point2D p = project(vs[i], d.W(), d.H(), 90.0f);  // FOV Magic number...
        // std::cout << "screen point:  (" << p.x << ", " << p.y << ")\n";
        if (p.z <= 0) {
            continue;
        }

        d.putPixel(
            p.x, p.y,
            i32rgba(0, 255, 0)
        );
    }
}

void wireframeRenderBFC(Display& d, Object& obj, std::vector<uint32_t>& visibleTris) {
    const std::vector<glm::vec3>& wvs = obj.worldVerts;

    for (uint32_t i : visibleTris) {
        Tri tri = obj.meshRenderer.triangles[i];

        glm::vec3 v1 = wvs[tri[0]], v2 = wvs[tri[1]], v3 = wvs[tri[2]];

        Point2D p1 = project(v1, d.W(), d.H(), 90.0f);
        Point2D p2 = project(v2, d.W(), d.H(), 90.0f);
        Point2D p3 = project(v3, d.W(), d.H(), 90.0f);

        // std::cout << "line: "  << p1.x << "," << p1.y << " -> " << p2.x << "," << p2.y << "\n";


        if (p1.z > 0 && p2.z > 0)  d.drawBresenhamLine(p1, p2, i32rgba(0,200,0));
        if (p3.z > 0 && p1.z > 0)  d.drawBresenhamLine(p3, p1, i32rgba(0,200,0));
        if (p2.z > 0 && p3.z > 0)  d.drawBresenhamLine(p2, p3, i32rgba(0,200,0));
    }
}

std::vector<uint32_t> cullBackFaces(Object& obj, Transform3D& tCamera, const std::vector<glm::vec3>& wvs) {
    std::vector<uint32_t> frontFacingTriIdxs;

    for (size_t i = 0; i < obj.meshRenderer.triangles.size(); i++) {
        Tri& tri = obj.meshRenderer.triangles[i];
        glm::vec3 a = wvs[tri[0]], b = wvs[tri[1]], c = wvs[tri[2]];

        glm::vec3 normal = glm::cross(b-a, c-a);  // * -1 since left handed xyz system

        if (glm::dot(normal, tCamera.pos - a) > 0) {
            frontFacingTriIdxs.emplace_back(i);
        }
    }

    return frontFacingTriIdxs;
}

std::vector<uint32_t> cullBackFacesScreen(Object& obj, const std::vector<Point2D>& projVs) {
    std::vector<uint32_t> frontFacingTriIdxs;

    for (size_t i = 0; i < obj.meshRenderer.triangles.size(); i++) {
        Tri& tri = obj.meshRenderer.triangles[i];
        Point2D a = projVs[tri[0]], b = projVs[tri[1]], c = projVs[tri[2]];

        int halfSigned2DArea = signedParallelogramArea(a,b,c);

        if (halfSigned2DArea > 0) {  // CCW
            frontFacingTriIdxs.emplace_back(i);

            // OR rasterize
        }
    }

    return frontFacingTriIdxs;
}

void backfaceCullZCullRasterizeLight(Display &d, Object& obj, const std::vector<Point2D>& projVs, const std::vector<Light*>& lights) {
    const int width = d.W();
    const int height = d.H();
    uint32_t* framebuffer = d.framebuffer.data();
    float* zbuffer = d.zbuffer.data();

    std::vector<Tri>& triangles = obj.meshRenderer.triangles;
    const std::vector<uint32_t>& colors = obj.meshRenderer.colors;

    // rasterize
    for (size_t i = 0; i < triangles.size(); i++) {
        Tri& tri = triangles[i];

        const Point2D p1 = projVs[tri[0]];
        const Point2D p2 = projVs[tri[1]];
        const Point2D p3 = projVs[tri[2]];

        const float area = signedParallelogramArea(p1, p2, p3);

        if (area >= 0)  // dependent on winding order
            continue;

        const int min_x = std::max(std::min({p1.x, p2.x, p3.x}), 0);
        const int max_x = std::min(std::max({p1.x, p2.x, p3.x}), width - 1);

        const int min_y = std::max(std::min({p1.y, p2.y, p3.y}), 0);
        const int max_y = std::min(std::max({p1.y, p2.y, p3.y}), height - 1);

        if (min_x > max_x || min_y > max_y)  // absurdities
            continue;

        const vec3i edge_dx{p2.y - p1.y,  p3.y - p2.y,  p1.y - p3.y};
        const vec3i edge_dy{p1.x - p2.x,  p2.x - p3.x,  p3.x - p1.x};

        Point2D start{min_x, min_y};

        vec3i edgeRow{
            signedParallelogramArea(p1, p2, start),
            signedParallelogramArea(p2, p3, start),
            signedParallelogramArea(p3, p1, start)
        };
        glm::vec3 zees{p1.z / area, p2.z / area, p3.z / area};
        // normalize with area.

        uint32_t color = colors[i];
        glm::vec3 a = obj.worldVerts[tri[0]];
        glm::vec3 b = obj.worldVerts[tri[1]];
        glm::vec3 c = obj.worldVerts[tri[2]];

        glm::vec3 normal = glm::normalize(glm::cross(b-a, c-a));

        Color shaded = intToColor(color);

        for (Light* l : lights) {
            if (auto* directional = dynamic_cast<DirectionalLight*>(l)) {
                glm::vec3 lightDir = glm::normalize(-directional->dir);

                float diffuse = glm::dot(normal, lightDir);
                diffuse = std::max(diffuse, 0.0f) * directional->intensity;

                float ambient = 0.5f;

                float intensity = std::max(ambient + diffuse, 0.0f);

                shaded = shade(shaded, directional->color, intensity);
            }
        }

        for (int y = min_y; y <= max_y; y++) {
            vec3i edge = edgeRow;

            uint32_t* frameRowbuf = framebuffer + y * width;
            float* zRowbuf = zbuffer + y * width;

            for (int x = min_x; x <= max_x; x++) {
                const bool insideTriangle = edge[0] <= 0 && edge[1] <= 0 && edge[2] <= 0;
                if (insideTriangle) {
                    float zTest = vec3i::dot(edge, zees);

                    if (*(zRowbuf + x) > zTest) {
                        *(frameRowbuf + x) = colorToInt(shaded);
                        *(zRowbuf + x) = zTest;
                    }
                }
                // interpolation step
                // TODO: lighting
                // step a pixel to rightwards
                edge += edge_dx;
            }
            // move a pixel down
            edgeRow += edge_dy;
        }
    }
}

void rasterizeFill(Display &d, Object& obj, std::vector<uint32_t>& visibleTris, const std::vector<Point2D>& projVs) {
    const int width = d.W();
    const int height = d.H();
    uint32_t* framebuffer = d.framebuffer.data();

    // rasterize
    for (uint32_t i : visibleTris) {
        Tri& tri = obj.meshRenderer.triangles[i];

        Point2D p1 = projVs[tri[0]];
        Point2D p2 = projVs[tri[1]];
        Point2D p3 = projVs[tri[2]];

        int min_x = std::max(std::min({p1.x, p2.x, p3.x}), 0);
        int max_x = std::min(std::max({p1.x, p2.x, p3.x}), width - 1);

        int min_y = std::max(std::min({p1.y, p2.y, p3.y}), 0);
        int max_y = std::min(std::max({p1.y, p2.y, p3.y}), height - 1);

        if (min_x > max_x || min_y > max_y)  // absurdities
            continue;

        int e1_dx = p2.y - p1.y;
        int e2_dx = p3.y - p2.y;
        int e3_dx = p1.y - p3.y;

        int e1_dy = p1.x - p2.x;
        int e2_dy = p2.x - p3.x;
        int e3_dy = p3.x - p1.x;

        Point2D start{min_x, min_y};

        int e1_row = signedParallelogramArea(p1, p2, start);
        int e2_row = signedParallelogramArea(p2, p3, start);
        int e3_row = signedParallelogramArea(p3, p1, start);

        for (int y = min_y; y <= max_y; y++) {
            int e1 = e1_row,  e2 = e2_row,  e3 = e3_row;

            uint32_t* row = framebuffer + y * width;

            for (int x = min_x; x <= max_x; x++) {
                if (e1 <= 0 && e2 <= 0 && e3 <= 0)
                    row[x] = obj.meshRenderer.colors[i];

                // move a pixel right
                e1 += e1_dx;
                e2 += e2_dx;
                e3 += e3_dx;
                
            }

            // move a pixel down
            e1_row += e1_dy;
            e2_row += e2_dy;
            e3_row += e3_dy;
        }
    }
}

Color shade(Color &material, Color &light, float &intensity) {
    const uint8_t rShade = std::clamp(material.r * (light.r / 255.0f) * intensity, 0.0f, 255.0f);
    const uint8_t gShade = std::clamp(material.g * (light.g / 255.0f) * intensity, 0.0f, 255.0f);
    const uint8_t bShade = std::clamp(material.b * (light.b / 255.0f) * intensity, 0.0f, 255.0f);
    
    return {rShade, gShade, bShade, material.a};
}

#pragma region OLD
void wireframeRenderNaive(Display& d, Object& obj) {
    std::vector<glm::vec3> vs = obj.worldVerts;

    for (size_t i = 0; i < obj.meshRenderer.triangles.size(); i++) {
        Tri& tri = obj.meshRenderer.triangles[i];

        glm::vec3 v1 = vs[tri[0]], v2 = vs[tri[1]], v3 = vs[tri[2]];

        Point2D p1 = project(v1, d.W(), d.H(), 90.0f);
        Point2D p2 = project(v2, d.W(), d.H(), 90.0f);
        Point2D p3 = project(v3, d.W(), d.H(), 90.0f);

        // std::cout << "line: "  << p1.x << "," << p1.y << " -> " << p2.x << "," << p2.y << "\n";


        if (p1.z > 0 && p2.z > 0)  d.drawBresenhamLine(p1, p2, i32rgba(0,200,0));
        if (p3.z > 0 && p1.z > 0)  d.drawBresenhamLine(p3, p1, i32rgba(0,200,0));
        if (p2.z > 0 && p3.z > 0)  d.drawBresenhamLine(p2, p3, i32rgba(0,200,0));
    }
}
#pragma endregion
