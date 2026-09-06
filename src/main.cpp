#include <stdio.h>
#include <iostream>
#include "display.hpp"
#include "raylib.h"
#include <cstdlib>
#include "rendering.hpp"
#include "components.hpp"
#include "utils.hpp"


int update(std::vector<Object>& objs);
int shaders(Display &d, Transform3D& cameraTransform, std::vector<Object>& objs);

int main() {
    int width = 500, height = 500, dS = 1;

    InitWindow(width*dS, height*dS, "Cenguine");
    Display display(width, height, dS, 0);

    std::vector<Object> objs {
        instantiate("objs/cube.obj", {0,0,2}, glm::quat({0, 1, 1}), {2,2,2}),
        // instantiate("objs/stanford-bunny.obj", {0,0,1.1}, glm::quat(), {5, 5, 5})
    };

    Transform3D t_Camera({0,0,0}, glm::identity<glm::quat>());

    objs[0].meshRenderer.randomizeTriColors();
    // objs[1].meshRenderer.randomizeTriColors();

    while (!WindowShouldClose()) {
        SetWindowTitle(TextFormat("Cenguine  |  FPS: %d", GetFPS()));

        BeginDrawing();

        // clear
        ClearBackground(BLACK); 
        display.clear(0);   

        update(objs);  // run behaviours (update)
        shaders(display, t_Camera, objs);  // run shaders

        // push Texture2D from framebuffer
        display.renderFramebuffer();

        EndDrawing();
    }

    std::cout << "Hello world, with raylib I think" << std::endl;

    return 0;
}

int update(std::vector<Object>& objs) {
    Object& o = objs[0];
    float dt = (float)GetFrameTime();
    
    o.translate({0,0,0.5*dt});
    // o.localRotateEuler({dt, dt, dt/2});

    // std::cout << o.pos.x << ", " << o.pos.y << ", " << o.pos.z << std::endl;

    return 0;
}

int shaders(Display &d, Transform3D& tCamera, std::vector<Object>& objs) {
    for (Object& o : objs) {
        auto vs = o.getWorldVerts();

        std::vector<uint32_t> visibleTris = cullBackFaces(o, tCamera, vs);
        // visibleTris.clear();
        // for (int i = 0; i < o.meshRenderer.triangles.size(); i++) {
        //     visibleTris.push_back(i);
        // }
        

        std::cout << "visible: " << visibleTris.size()
          << " / " << o.meshRenderer.triangles.size() << '\n';

        // wireframeRenderBFC(d, o, visibleTris, vs);
        rasterizeFill(d, o, visibleTris, tCamera, vs);
        // vertexRender(d, vs);
    }

    return 0;
}

