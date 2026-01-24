#pragma once

#include "object.h"

#include <vector>

// This only supports triangular meshes
class Mesh : public Object
{
public:
    Mesh(Material m, std::vector<glm::vec3> points, glm::mat4 modelT);

    bool intersect(Ray ray, RayIntersection& out) const override;

    std::vector<glm::vec3> points;
    glm::mat4 modelT;
};