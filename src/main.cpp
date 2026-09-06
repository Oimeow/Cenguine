#include <stdio.h>
#include <iostream>
#include "display.hpp"
#include "raylib.h"
#include <cstdlib>
#include "rendering.hpp"
#include "components.hpp"

int update(std::vector<Object>& objs);
int shaders(Display &d, const std::vector<Object>& objs);

int main() {
    int width = 1000, height = 1000, dS = 1;

    InitWindow(width*dS, height*dS, "Cenguine");
    Display display(width, height, dS, 0);

    std::vector<Object> objs {
        instantiate("objs/cube.obj", {0,0,0})
    };

    while (!WindowShouldClose()) {
        SetWindowTitle(TextFormat("Cenguine  |  FPS: %d", GetFPS()));

        BeginDrawing();

        // clear
        ClearBackground(BLACK); 
        display.clear(0);   

        update(objs);  // run behaviours (update)
        shaders(display, objs);  // run shaders

        // push Texture2D from framebuffer
        display.renderFramebuffer();

        EndDrawing();
    }

    std::cout << "Hello world, with raylib I think" << std::endl;

    return 0;
}

int update(std::vector<Object>& objs) {
    Object& o = objs[0];
    
    o.translate({0,0,1.0f/60.0f});

    return 0;
}

int shaders(Display &d, const std::vector<Object>& objs) {
    for (const Object& o : objs) {

        std::cout << "before wireframe\n";
        wireframeRender(d, const_cast<Object&>(o));
        std::cout << "after wireframe\n";
        // vertexRender(d, o.getWorldVerts());
    }

    return 0;
}

