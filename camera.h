#pragma once

#include <string>

#include <glm/glm.hpp>

#include "world.h"

class Camera
{
public:
    Camera(glm::vec3 position,
        glm::vec3 lookAt,
        glm::vec3 up,
        glm::vec3 clearColor,
        float filmHeight,
        float focalLen = 1.0f);

    void render(const World& world, std::string filename, int width, int height);

    glm::vec3 clearColor;
    glm::vec3 position;
    glm::vec3 lookAt;
    glm::vec3 up;
    float filmHeight;
    float focalLen;
};