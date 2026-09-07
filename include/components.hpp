#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "utils.hpp"


class MeshRenderer {
public:
    std::vector<glm::vec3> vertices;
    std::vector<Tri> triangles;
    std::string meshName;
    std::vector<uint32_t> colors;

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

    MeshRenderer meshRenderer;

    void translate(const glm::vec3& vec);
    void localRotateEuler(const glm::vec3& eulerAngles);
    void updateWorldVerts();

private:
     
};


Object instantiate(
    const std::string& objFile, 
    glm::vec3 pos = {0,0,0}, 
    glm::quat rot = glm::identity<glm::quat>(),
    glm::vec3 scale = {1,1,1}
);