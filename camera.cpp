#include "camera.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

Camera::Camera(glm::vec3 position,
    glm::vec3 lookAt,
    glm::vec3 up,
    float focalLen): 
        position(position),
        lookAt(lookAt),
        up(up),
        focalLen(focalLen){}

void Camera::render(const World& world,
    std::string filename,
    int width,
    int height)
{
    // TODO: Write actual render logic lol

    vector<unsigned char> image = vector<unsigned char>(width * height * 3);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (x % 4 == y % 4)
            {
                image[(x + y * width) * 3] = 255;
            }
            else
            {
                image[(x + y * width) * 3 + 2] = 255;
            }
        }
    }

    stbi_write_png(filename.c_str(), width, height, 3, image.data(), 3 * width);
}