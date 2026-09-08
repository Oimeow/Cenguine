#include "components.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include "utils.hpp"
#include "rendering.hpp"

// for randomness
#include <cstdlib>
#include <ctime>
#include <cassert>


FaceVertex parseFaceVertex(const std::string& face) {
    FaceVertex res;

    std::stringstream ss(face);
    
    std::string v, vt, vn;

    std::getline(ss, v, '/');
    std::getline(ss, vt, '/');
    std::getline(ss, vn, '/');

    // recall that .OBJ files are 1-indexed
    res.vertex = std::stoi(v) - 1;

    res.texcoord = !vt.empty() ? std::stoi(vt) - 1 : -1;
    res.normal = !vn.empty() ? std::stoi(vn) - 1 : -1;

    return res;
}

// class Scene

Object Scene::instantiate(
    const std::string& objFile,
    glm::vec3 pos, 
    glm::quat rot, 
    glm::vec3 scale
) 
{
    Object o = Object::instantiate(objFile, pos, rot, scale);
    objects.push_back(o);
    return o;
}


// class Object

void Object::translate(const glm::vec3& vec) {
    pos += vec;
}

void Object::localRotateEuler(const glm::vec3& eulerAngles) {
    glm::quat transformation = glm::quat(eulerAngles);

    rotation *= transformation;
}


void Object::updateWorldVerts() {
    worldVerts.resize(meshRenderer.vertices.size());

    for (size_t i = 0; i < meshRenderer.vertices.size(); i++) {
        // local translations/transformations
        glm::vec3 vf = meshRenderer.vertices[i] * localScale;
        vf = rotation * vf;
        vf += pos;

        worldVerts[i] = vf;
    }
}

void Object::debugObjectInformation() {
    std::cout << "==== Debug (" << meshRenderer.meshName << ") ====\n";
    std::cout << "Verts / Tris / Colors  :  " << std::dec << 
                meshRenderer.vertices.size() << " / " <<
                meshRenderer.triangles.size() << " / " <<
                meshRenderer.colors.size() <<
                "\n";
    std::cout << "pos | rot  :  " <<
                vecAsString(pos) << " | " <<
                quatAsString(rotation) << std::endl;
}

Object Object::instantiate(
    const std::string& objFile,
    glm::vec3 pos, 
    glm::quat rot, 
    glm::vec3 scale
) {
    Object obj;

    obj.meshRenderer = MeshRenderer::loadFromObj(objFile);
    obj.pos = pos;
    obj.rotation = rot;
    obj.localScale = scale;

    return obj;
}


// class MeshRenderer

void MeshRenderer::initTriColors(Color col) {
    colors.clear();
    colors.reserve(triangles.size());

    uint32_t color_int = colorToInt(col);

    for (Tri tri : triangles) {
        colors.push_back(color_int);
    }
}

void MeshRenderer::randomizeTriColors() {
    colors.clear();
    colors.reserve(triangles.size());

    srand(time(nullptr));
    for (size_t i = 0; i < triangles.size(); i++) {
        uint8_t r = rand() % 256;
        uint8_t g = rand() % 256;
        uint8_t b = rand() % 256;
        // std::cout << (r << 16 | g << 8 | b) << std::endl;
        colors.push_back(i32rgba(r, g, b));
    }

    std::cout << std::endl;
    for (auto color : colors) {
        std::cout << color << std::endl;
    }

    assert(colors.size() == triangles.size());
}

MeshRenderer MeshRenderer::loadFromObj(std::string objFname) {
    MeshRenderer result;

    std::ifstream fr(objFname);

    if (!fr) {
        std::cout << "Failed to read " << objFname << std::endl;
        return result;
    }

    std::vector<glm::vec3> vs;
    std::vector<Tri> tris;
    std::string _meshName;

    // iterate through lines
    std::string line;
    while (std::getline(fr, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));  // .lstrip()

        if (line[0] == 'o') {
            std::stringstream ss(line);
            char type;
            
            ss >> type >> _meshName;
        }
        else if (line[0] == 'v') {
            // vertex handling
            std::stringstream ss(line);
            char type;
            float x,y,z;

            ss >> type >> x >> y >> z;

            vs.push_back({x,y,z});
        }
        else if (line[0] == 'f') {
            // face/tri processing
            std::stringstream ss(line);
            char type;
            ss >> type;

            // chop up into "v/vt/vn" groups
            std::vector<FaceVertex> fvGroup;
            std::string face;
            while (ss >> face) {
                FaceVertex fv = parseFaceVertex(face);
                fvGroup.push_back(fv);

                if (fvGroup.size() < 3) { continue; }

                tris.emplace_back(
                    fvGroup[0].vertex,
                    fvGroup[fvGroup.size() - 2].vertex, 
                    fvGroup[fvGroup.size() - 1].vertex
                );
                // fanning like this only works on convex faces.
                // concave faces require something more proper.
            }
        }
    }

    result.vertices = vs;
    result.triangles = tris;
    result.meshName = _meshName;

    return result;
}

