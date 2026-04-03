#pragma once

#include <glm/glm.hpp>

struct Light
{
    glm::vec3 position;

    // color in RADIANCE
    glm::vec3 color;

    // TODO: remove color above
    glm::vec3 power;
};