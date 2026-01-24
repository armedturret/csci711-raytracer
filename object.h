#pragma once

#include <vector>

#include "material.h"
#include "ray.h"

class Object
{
public:
    Object(Material m) : material(m) {}

    virtual bool intersect(Ray ray, RayIntersection& out) = 0;

    Material material;
};