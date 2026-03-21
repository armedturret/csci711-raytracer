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
        float filmHeight,
        float focalLen = 1.0f);

    void render(const World& world, std::string filename, int width, int height, bool superSample = false, int threadCount = 1) const;

    glm::vec3 position;
    glm::vec3 lookAt;
    glm::vec3 up;
    float filmHeight;
    float focalLen;

private:
    // Function that renders one part of the entire image
    void RenderRegion(int startRow,
        int endRow,
        uint8_t* image,
        int imageWidth,
        bool superSample,
        const glm::mat4& inverseViewT,
        const float& filmWidth,
        const float& pixelSize,
        const World& world) const;

    // Generates a ray through the center of the pixel
    Ray generateWorldspaceRay(const glm::ivec2& pixel,
        const glm::vec2& viewOffset,
        const glm::mat4& inverseViewT,
        const float& filmWidth,
        const float& pixelSize) const;

    // Samples a given ray returns a color 1-0
    glm::vec3 sampleRay(const Ray& ray, const World& world) const;
};