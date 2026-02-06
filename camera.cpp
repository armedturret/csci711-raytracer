#include "camera.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

Camera::Camera(glm::vec3 position,
    glm::vec3 lookAt,
    glm::vec3 up,
    glm::vec3 clearColor,
    float filmHeight,
    float focalLen) :
    position(position),
    lookAt(lookAt),
    up(up),
    clearColor(clearColor),
    filmHeight(filmHeight),
    focalLen(focalLen)
{
}

void Camera::render(const World& world,
    std::string filename,
    int width,
    int height,
    bool superSample)
{
    auto viewTransform = glm::lookAt(position, lookAt, up);
    auto inverseViewTransform = glm::transpose(glm::inverse(viewTransform));

    auto image = vector<uint8_t>(width * height * 3);
    float pixelSize = filmHeight / (float)height;
    float filmWidth = filmHeight * (float)width / (float)height;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            glm::vec3 color = glm::vec3(0.0f);

            if (superSample)
            {
                float qPix = pixelSize / 4.0f;
                for (int i = 0; i < 4; i++)
                {
                    Ray ray = generateWorldspaceRay(glm::ivec2(x, y),
                        glm::vec2(i % 2 == 0 ? qPix : -qPix, i < 2 ? qPix : -qPix),
                        inverseViewTransform,
                        filmWidth,
                        pixelSize);
                    color += sampleRay(ray, world);
                }
                color /= 4.0f;
            }
            else
            {
                Ray ray = generateWorldspaceRay(glm::ivec2(x, y),
                    glm::vec2(0.0f),
                    inverseViewTransform,
                    filmWidth,
                    pixelSize);
                color = sampleRay(ray, world);
            }

            image[(x + y * width) * 3] = (uint8_t)(color.r * 255.0f);
            image[(x + y * width) * 3 + 1] = (uint8_t)(color.g * 255.0f);
            image[(x + y * width) * 3 + 2] = (uint8_t)(color.b * 255.0f);
        }
    }

    stbi_write_png(filename.c_str(), width, height, 3, image.data(), 3 * width);
}

Ray Camera::generateWorldspaceRay(const glm::ivec2& pixel,
    const glm::vec2& viewOffset,
    const glm::mat4& inverseViewT,
    const float& filmWidth,
    const float& pixelSize)
{
    Ray ray;
    ray.origin = position;

    ray.origin = position;

    // define direction in view space, then convert to world space
    ray.direction = glm::vec3(
        -filmWidth / 2.0f + pixelSize / 2.0f + (float)pixel.x * pixelSize + viewOffset.x,
        filmHeight / 2.0f - pixelSize / 2.0f - (float)pixel.y * pixelSize + viewOffset.y,
        focalLen);
    ray.direction = glm::normalize(ray.direction);
    ray.direction = inverseViewT * glm::vec4(ray.direction, 1.0f);
    ray.direction = glm::normalize(ray.direction);
    // opengl standard is a little silly, need to spin the ray around
    ray.direction.z *= -1;
    return ray;
}

glm::vec3 Camera::sampleRay(const Ray& ray, const World& world)
{
    glm::vec3 color = clearColor;

    // adhoc conversion method
    float maxIrradiance = 1.0f;

    RayIntersection hit;
    if (world.raycast(ray, hit, false, focalLen))
    {
        // Very simple tone production for now
        color = hit.irradiance / maxIrradiance;
        color = glm::clamp(color, 0.0f, 1.0f);
    }
    return color;
}