#pragma once

#include <string>

#include <glm/glm.hpp>

#include "world.h"

enum class ToneReproduction { LINEAR, WARD, REINHARD };

class Camera
{
public:
    Camera(glm::vec3 position,
        glm::vec3 lookAt,
        glm::vec3 up,
        float filmHeight,
        float focalLen = 1.0f,
        float maxPhotonSampleDistance = 10.0f,
        int maxPhotonSampleCount = 15);

    void render(const World& world,
        std::string filename,
        int width,
        int height,
        bool superSample = false,
        int threadCount = 1,
        ToneReproduction reproMethod = ToneReproduction::LINEAR,
        float ldmax = 1.0f) const;

    glm::vec3 position;
    glm::vec3 lookAt;
    glm::vec3 up;
    float filmHeight;
    float focalLen;
    float maxPhotonSampleDistance;
    int maxPhotonSampleCount;

private:
    // Function that renders one part of the entire image
    void RenderRegion(int startRow,
        int endRow,
        glm::vec3* irradianceMap,
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
};