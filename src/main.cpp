#include <stdio.h>
#include <iostream>
#include "display.hpp"
#include "raylib.h"
#include <cstdlib>
#include "rendering.hpp"
#include "components.hpp"
#include "utils.hpp"


int update(Scene &scene);
int shaders(Display &d, Scene &scene, CCamera& camera);

int main() {
    int width = 500, height = 500, dS = 1;

    InitWindow(width*dS, height*dS, "Cenguine");
    Display display(width, height, dS, 0);

    std::vector<Object> objs {
        Object::instantiate("objs/cube.obj", {0,0,5}, glm::quat({0, 1, 1}), {2,2,2}),
        Object::instantiate("objs/stanford-bunny.obj",{0,0,1.1},glm::quat({1,0,0,0}), {5,5,5})
    };

    DirectionalLight sun = DirectionalLight(Color(255, 255, 255), {0, 1, 0}, 1.0f);

    std::vector<Light*> lights {
        &sun
    };

    Scene activeScene{objs, lights, "testing grounds"};

    CCamera cam({0,0,0});

    for (auto& obj : activeScene.objects) {
        obj.meshRenderer.initTriColors({255,255,255,255});
        //obj.meshRenderer.randomizeTriColors();    
    }

    while (!WindowShouldClose()) {
        SetWindowTitle(TextFormat("Cenguine  |  FPS: %d", GetFPS()));

        BeginDrawing();

        // clear
        // ClearBackground(BLACK); 
        display.clear(0xff000000);   

        update(activeScene);  // run behaviours (update)
        shaders(display, activeScene, cam);  // run shaders

        // push Texture2D from framebuffer
        display.renderFramebuffer();

        EndDrawing();
    }

    std::cout << "Hello world, with raylib I think" << std::endl;

    return 0;
}

int update(Scene& scene) {
    Object& o = scene.objects[0];
    float dt = (float)GetFrameTime();
    
    o.translate({0,0,0.5*dt});
    scene.objects[1].localRotateEuler({0, dt, 0});

    // std::cout << o.pos.x << ", " << o.pos.y << ", " << o.pos.z << std::endl;

    return 0;
}

int shaders(Display &d, Scene& scene, CCamera& camera) {
    const int width = d.W();
    const int height = d.H();

    Projection fproj = fov_to_f(height, camera.fov);

    for (Object& o : scene.objects) {
        o.updateWorldVerts();
        const auto& worldVs = o.worldVerts;

        o.debugObjectInformation();

        std::vector<Point2D> projVs(worldVs.size());
        for (size_t i = 0; i < worldVs.size(); i++) {
            glm::vec3 camV = worldVs[i];

            camV -= camera.pos;
            camV = camera.rotation * camV;

            projVs[i] = project(camV, width, height, fproj);
        }

        cullAndRasterizeWithLighting(d, o, projVs, scene.lights);
        // std::vector<uint32_t> visibleTris = cullBackFacesScreen(o, projVs);

        // std::cout << "visible: " << visibleTris.size()
        //   << " / " << o.meshRenderer.triangles.size() << '\n';

        // rasterizeFill(d, o, visibleTris, projVs);
        // vertexRender(d, worldVs);
    }

    return 0;
}

