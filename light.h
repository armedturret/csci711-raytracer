#pragma once

#include <glm/glm.hpp>

struct Light
{
    glm::vec3 position;

    // power in RADIANCE
    glm::vec3 power;
};