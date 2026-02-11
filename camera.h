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

    void render(const World& world, std::string filename, int width, int height, bool superSample = false, int threadCount = 1);

    glm::vec3 clearColor;
    glm::vec3 position;
    glm::vec3 lookAt;
    glm::vec3 up;
    float filmHeight;
    float focalLen;

private:
    // Function that renders one part of the entire image
    void RenderRegion(size_t startRow,
        size_t endRow,
        uint8_t* image,
        size_t imageWidth,
        bool superSample,
        const glm::mat4& inverseViewT,
        const float& filmWidth,
        const float& pixelSize,
        const World& world);

    // Generates a ray through the center of the pixel
    Ray generateWorldspaceRay(const glm::ivec2& pixel,
        const glm::vec2& viewOffset,
        const glm::mat4& inverseViewT,
        const float& filmWidth,
        const float& pixelSize);

    // Samples a given ray returns a color 1-0
    glm::vec3 sampleRay(const Ray& ray, const World& world);
};