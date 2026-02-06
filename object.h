#pragma once

#include <vector>

#include "material.h"
#include "ray.h"

class Object
{
public:
    Object(Material* material) : material(material) {}

    /// <summary>
    /// Returns the closest (non-negative) intersection
    /// </summary>
    /// <param name="ray"></param>
    /// <param name="out"></param>
    /// <returns></returns>
    virtual bool intersect(Ray ray, RayIntersection& out) const = 0;

    Material* material;
};