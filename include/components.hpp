#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "utils.hpp"
#include "string.h"


class MeshRenderer {
public:
    std::vector<glm::vec3> vertices;
    std::vector<Tri> triangles;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> vertexNormals;
    std::string meshName;
    std::vector<CColor> colors;

    void initTriColors(CColor col);
    void randomizeTriColors();
    static MeshRenderer loadFromObj(std::string objFname);

private:
    
};

class Object {
public:
    glm::vec3 pos = glm::vec3{0,0,0};
    glm::quat rotation = glm::identity<glm::quat>();
    glm::vec3 localScale{1.0f};
    std::vector<glm::vec3> worldVerts;
    std::vector<glm::vec3> worldVertNormals;

    MeshRenderer meshRenderer;

    void translate(const glm::vec3& vec);
    void localRotateEuler(const glm::vec3& eulerAngles);
    void updateWorldVerts();
    void updateWorldNormals();
    void debugObjectInformation();

    static Object instantiate(const std::string& objFile, glm::vec3 pos = {0,0,0}, glm::quat rot = glm::identity<glm::quat>(), glm::vec3 scale = {1,1,1});

private:
    // want to add a "needsToMove" bool, but that is influenced by camera as well.
};

class Light {
public:
    CColor color{255,255,255,255};
    float intensity{1.0f};

    Light(CColor color, float intensity)
        : color(color), intensity(intensity)
    {}

    virtual ~Light() = default;
private:

};

class DirectionalLight : public Light {
public:
    glm::vec3 dir;

    DirectionalLight(const CColor &color, const glm::vec3 &dir, float intensity)
        : Light(color, intensity), dir(glm::normalize(dir))
    {}

private:

};

class Scene {
public:
    std::vector<Object> objects{};
    std::vector<Light*> lights{};
    std::string getSceneName() { return sceneName; }

    Scene(std::vector<Object>& objects, std::vector<Light*>& lights, std::string sceneName)
      : objects(objects), lights(lights), sceneName(sceneName)
    {}

    static Scene readSceneFromFile();
    Object instantiate(const std::string& objFile, glm::vec3 pos = {0,0,0}, glm::quat rot = glm::identity<glm::quat>(), glm::vec3 scale = {1,1,1});

private:
    uint32_t sceneIndex;
    std::string sceneName;
};

class CCamera {
public:
    glm::vec3 pos;
    glm::quat rotation;
    float fov;

    CCamera(glm::vec3 pos, glm::quat rotation = glm::identity<glm::quat>(), float fov=90)
        : pos(pos), rotation(rotation), fov(fov)
    {}

private:

};

