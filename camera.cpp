#include "camera.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

Camera::Camera(glm::vec3 position,
    glm::vec3 lookAt,
    glm::vec3 up,
    float filmHeight,
    float focalLen) :
    position(position),
    lookAt(lookAt),
    up(up),
    filmHeight(filmHeight),
    focalLen(focalLen)
{
}

void Camera::render(const World& world,
    std::string filename,
    int width,
    int height)
{
    auto viewTransform = glm::lookAt(position, lookAt, up);
    auto inverseViewTransform = glm::inverse(viewTransform);

    glm::vec3 clearColor(0.0f, 0.0f, 0.0f);

    glm::vec3 testPos(0.0f, 0.0f, 1.0f);
    testPos = inverseViewTransform * glm::vec4(testPos, 1.0f);

    auto image = vector<uint8_t>(width * height * 3);
    float pixelSize = filmHeight / (float)height;
    float filmWidth = filmHeight * (float)width / (float)height;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            glm::vec3 color = clearColor;

            // construct a ray in WORLD SPACE
            Ray ray;
            ray.origin = position;

            // define direction in view space, then convert to world space
            ray.direction = glm::vec3(
                -filmWidth / 2.0f + pixelSize / 2.0f + (float)x * pixelSize,
                filmHeight / 2.0f - pixelSize / 2.0f - (float)y * pixelSize,
                -focalLen);
            ray.direction = glm::transpose(inverseViewTransform) * glm::vec4(ray.direction, 1.0f);
            ray.direction = glm::normalize(ray.direction);

            RaycastHit hit;
            if (world.raycast(ray, hit, focalLen))
            {
                // Very simple tone production for now
                color = hit.hit->material.color;
            }

            image[(x + y * width) * 3] = (uint8_t)(color.r * 255.0f);
            image[(x + y * width) * 3 + 1] = (uint8_t)(color.g * 255.0f);
            image[(x + y * width) * 3 + 2] = (uint8_t)(color.b * 255.0f);
        }
    }

    stbi_write_png(filename.c_str(), width, height, 3, image.data(), 3 * width);
}