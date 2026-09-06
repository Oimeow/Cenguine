#include "rendering.hpp"
#include <iostream>
#include "components.hpp"
#include "utils.hpp"
#include <algorithm>


Point2D project(const glm::vec3& v, int w, int h, float fov, float clippingPlane) {
    if (v.z <= clippingPlane) {
        return {0,0,v.z};
    }

    float f = 0.5f*h / glm::tan(0.5f * glm::radians(fov));
    int xf = int(0.5f * w + v.x * f / v.z);
    int yf = int(0.5f * h - v.y * f / v.z);

    return {xf, yf, v.z};
}

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

void wireframeRenderBFC(Display& d, Object& obj, std::vector<uint32_t>& visibleTris, const std::vector<glm::vec3>& wvs) {
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

void rasterizeFill(Display &d, Object& obj, std::vector<uint32_t>& visibleTris, Transform3D& tCamera, const std::vector<glm::vec3>& wvs) {
    for (uint32_t i : visibleTris) {
        Tri tri = obj.meshRenderer.triangles[i];
        glm::vec3 v1 = wvs[tri[0]], v2 = wvs[tri[1]], v3 = wvs[tri[2]];

        Point2D p1 = project(v1, d.W(), d.H(), 90.0f);
        Point2D p2 = project(v2, d.W(), d.H(), 90.0f);
        Point2D p3 = project(v3, d.W(), d.H(), 90.0f);

        int min_x = std::max(std::min({p1.x, p2.x, p3.x}), 0);
        int max_x = std::min(std::max({p1.x, p2.x, p3.x}), int(d.W() - 1));

        int min_y = std::max(std::min({p1.y, p2.y, p3.y}), 0);
        int max_y = std::min(std::max({p1.y, p2.y, p3.y}), int(d.H() - 1));

        for (int y = min_y; y < max_y; y++) {
            for (int x = min_x; x < max_x; x++) {
                Point2D xy = Point2D(x,y);
                if (!rightOfEdgeAB(p1,p2,xy) && !rightOfEdgeAB(p2,p3,xy) && !rightOfEdgeAB(p3,p1,xy)) {
                    if (x < 0 || x >= d.W() || y < 0 || y >= d.H()) {
                        std::cerr << "BAD PIXEL: " << x << ", " << y << "\n";
                        abort();
                    }

                    assert(i < obj.meshRenderer.colors.size());
                    d.putPixel(xy, obj.meshRenderer.colors[i]);
                }
            }
        }

    }
}



#pragma region OLD
void wireframeRenderNaive(Display& d, Object& obj) {
    std::vector<glm::vec3> vs = obj.getWorldVerts();

    for (size_t i = 0; i < obj.meshRenderer.triangles.size(); i++) {
        Tri tri = obj.meshRenderer.triangles[i];

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
