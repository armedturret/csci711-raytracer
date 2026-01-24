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
        float focalLen = 1.0f):
        position(position),
        lookAt(lookAt),
        up(up),
        focalLen(focalLen){}

    void render(World world, std::string filename, int width, int height);

    glm::vec3 position;
    glm::vec3 lookAt;
    glm::vec3 up;
    float focalLen;
};