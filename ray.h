#pragma once

#include <glm/glm.hpp>

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
};

struct RayIntersection
{
    glm::vec3 position;
    glm::vec3 normal;
};
