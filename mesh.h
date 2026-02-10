#pragma once

#include "triangle.h"
#include "material.h"

#include <vector>
#include <string>
#include <memory>

class Mesh
{
public:
    Mesh(Material* m, const glm::mat4& modelT, std::vector<glm::vec3> points);

    Mesh(Material* m, const glm::mat4& modelT, std::string filePath);

    const std::vector<std::unique_ptr<Triangle>>& getTriangles() { return triangles; }
private:
    void init(const glm::mat4& modelT, const std::vector<glm::vec3>& points);

    std::vector<std::unique_ptr<Triangle>> triangles;
    Material* m;
};