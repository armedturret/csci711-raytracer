#pragma once

#include "light.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
};

struct RayIntersection
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    // Incoming ray direction (points TO intersection)
    glm::vec3 incoming;
    float distance;

    // Only calculated if not a terminal ray
    std::vector<Light*> visibleLights;

    // Filled in by material
    glm::vec3 irradiance;
};
