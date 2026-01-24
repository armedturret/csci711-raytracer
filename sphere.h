#pragma once

#include "object.h"

class Sphere : public Object
{
public:
    Sphere(Material m, glm::vec3 center, float radius);

    bool intersect(Ray ray, RayIntersection& out) override;

    glm::vec3 center;
    float radius;
};