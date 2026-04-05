#include "camera.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>

#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

Camera::Camera(glm::vec3 position,
    glm::vec3 lookAt,
    glm::vec3 up,
    float filmHeight,
    float focalLen,
    float maxPhotonSampleDistance,
    int maxPhotonSampleCount) :
    position(position),
    lookAt(lookAt),
    up(up),
    filmHeight(filmHeight),
    focalLen(focalLen),
    maxPhotonSampleDistance(maxPhotonSampleDistance),
    maxPhotonSampleCount(maxPhotonSampleCount)
{
}

void Camera::render(const World& world,
    std::string filename,
    int width,
    int height,
    bool superSample,
    int threadCount) const
{
    cout << "Rendering " << filename << "..." << endl;
    auto startTime = chrono::system_clock::now();

    auto viewTransform = glm::lookAt(position, lookAt, up);
    auto inverseViewTransform = glm::transpose(glm::inverse(viewTransform));

    auto image = vector<uint8_t>(width * height * 3);
    auto irradianceMap = vector<glm::vec3>(width * height);
    float pixelSize = filmHeight / static_cast<float>(height);
    float filmWidth = filmHeight * static_cast<float>(width) / static_cast<float>(height);

    if (threadCount == 1)
    {
        // Just render the full thing synchronously (useful for debugging)
        RenderRegion(0,
            height,
            irradianceMap.data(),
            width,
            superSample,
            inverseViewTransform,
            filmWidth,
            pixelSize,
            world);
    }
    else
    {
        vector<thread> renderThreads;

        int sizePerThread = static_cast<int>(glm::ceil(static_cast<float>(height) / static_cast<float>(threadCount)));

        for (int y = 0; y < height; y += sizePerThread)
        {
            int endRow = y + sizePerThread;
            if (endRow > height)
            {
                endRow = height;
            }

            renderThreads.emplace_back(mem_fn(&Camera::RenderRegion),
                this,
                y,
                endRow,
                irradianceMap.data(),
                width,
                superSample,
                inverseViewTransform,
                filmWidth,
                pixelSize,
                world);
        }

        for (int i = 0; i < renderThreads.size(); i++)
        {
            renderThreads[i].join();
        }
    }

    float maxIrradiance = 0.0f;
    // find the maxium irradiance in the entire image (accross all channels)
    for (auto ir : irradianceMap)
    {
        for (int c = 0; c < 3; c++)
        {
            maxIrradiance = glm::max(ir[c], maxIrradiance);
        }
    }
    // TODO: remove this and replace w/ proper tone mapping (this is so dumb)
    maxIrradiance = 0.0001f;

    // write the irradiances to the image data
    for (int i = 0; i < irradianceMap.size(); i++)
    {
        image[i * 3] = (uint8_t)(glm::clamp(irradianceMap[i].r / maxIrradiance, 0.0f, 1.0f) * 255.0f);
        image[i * 3 + 1] = (uint8_t)(glm::clamp(irradianceMap[i].g / maxIrradiance, 0.0f, 1.0f) * 255.0f);
        image[i * 3 + 2] = (uint8_t)(glm::clamp(irradianceMap[i].b / maxIrradiance, 0.0f, 1.0f) * 255.0f);
    }

    stbi_write_png(filename.c_str(), width, height, 3, image.data(), 3 * width);

    auto endTime = chrono::system_clock::now();
    chrono::duration<float> renderTime = endTime - startTime;
    cout << "Finished rendering in " << renderTime.count() << " seconds" << endl;
}

void Camera::RenderRegion(int startRow,
    int endRow,
    glm::vec3* irradianceMap,
    int imageWidth,
    bool superSample,
    const glm::mat4& inverseViewT,
    const float& filmWidth,
    const float& pixelSize,
    const World& world) const
{
    for (int y = static_cast<int>(startRow); y < endRow; y++)
    {
        for (int x = 0; x < imageWidth; x++)
        {
            glm::vec3 irradiance = glm::vec3(0.0f);

            if (superSample)
            {
                float qPix = pixelSize / 4.0f;
                for (int i = 0; i < 4; i++)
                {
                    Ray ray = generateWorldspaceRay(glm::ivec2(x, y),
                        glm::vec2(i % 2 == 0 ? qPix : -qPix, i < 2 ? qPix : -qPix),
                        inverseViewT,
                        filmWidth,
                        pixelSize);
                    irradiance += world.illuminate(ray, maxPhotonSampleDistance, maxPhotonSampleCount, focalLen);
                }
                irradiance /= 4.0f;
            }
            else
            {
                Ray ray = generateWorldspaceRay(glm::ivec2(x, y),
                    glm::vec2(0.0f),
                    inverseViewT,
                    filmWidth,
                    pixelSize);
                irradiance = world.illuminate(ray, maxPhotonSampleDistance, maxPhotonSampleCount, focalLen);
            }

            irradianceMap[(x + y * imageWidth)] = irradiance;
        }
    }
}

Ray Camera::generateWorldspaceRay(const glm::ivec2& pixel,
    const glm::vec2& viewOffset,
    const glm::mat4& inverseViewT,
    const float& filmWidth,
    const float& pixelSize) const
{
    Ray ray;
    ray.origin = position;

    // define direction in view space, then convert to world space
    ray.direction = glm::vec3(
        -filmWidth / 2.0f + pixelSize / 2.0f + static_cast<float>(pixel.x) * pixelSize + viewOffset.x,
        filmHeight / 2.0f - pixelSize / 2.0f - static_cast<float>(pixel.y) * pixelSize + viewOffset.y,
        focalLen);
    ray.direction = glm::normalize(ray.direction);
    ray.direction = inverseViewT * glm::vec4(ray.direction, 1.0f);
    ray.direction = glm::normalize(ray.direction);
    // opengl standard is a little silly, need to spin the ray around
    ray.direction.z *= -1;
    return ray;
}