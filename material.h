#pragma once

#include <glm/glm.hpp>

struct RayIntersection;

class Material
{
public:
    /// <summary>
    /// Calculates and fills in the reflected radiance in the hit data.
    /// </summary>
    /// <param name="hit">Existing hit data</param>
    virtual void illuminate(RayIntersection* hit) = 0;

    // reflective component
    float kr = 0.0f;

    // transmissive component
    float kt = 0.0f;
};