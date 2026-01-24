#pragma once

#include <vector>

#include "material.h"
#include "ray.h"

class Object
{
public:
    std::vector<RayIntersection> intersect(Ray ray);
};