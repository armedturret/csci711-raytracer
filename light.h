#pragma once

#include <glm/glm.hpp>

struct Light
{
    glm::vec3 position;

    // power in RADIANCE
    glm::vec3 power;

    // hack to add spotlights :P
    bool isSpotLight;
    glm::vec3 dir;
    // angle of light in degrees
    float angle;

    Light(glm::vec3 position,
        glm::vec3 power,
        bool isSpotLight = false,
        glm::vec3 dir = glm::vec3(1.0f),
        float angle = 60.0f) :
        position(position),
        power(power),
        isSpotLight(isSpotLight),
        dir(glm::normalize(dir)),
        angle(angle)
    {
    }
};