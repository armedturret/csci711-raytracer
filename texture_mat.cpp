#include "texture_mat.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"

#include "ray.h"

using namespace std;

TextureMaterial::TextureMaterial(const std::string& imgPath,
    glm::vec3 specular,
    float kd,
    float ks,
    float ke,
    float kr,
    float kt) :
    PhongMaterial(specular, kd, ks, ke, kr, kt)
{
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(imgPath.c_str(), &size.x, &size.y, &channels, 3);

    if (!data)
    {
        cout << "Failed to load image from " << imgPath << endl;
        return;
    }
}

TextureMaterial::~TextureMaterial()
{
    if (data)
    {
        stbi_image_free(data);
    }
}

glm::vec3 TextureMaterial::getDiffuseCoefficients(RayIntersection* hit)
{
    glm::ivec2 coords;
    coords.x = static_cast<int>(hit->uv.x * size.x) % size.x;
    coords.y = static_cast<int>(hit->uv.y * size.y) % size.y;
    if (coords.x < 0)
        coords.x += size.x;
    if (coords.y < 0)
        coords.y += size.y;
    glm::vec3 color;
    int start = (coords.x + coords.y * size.x) * channels;
    color.x = static_cast<float>(data[start]) / 255.0f;
    color.y = static_cast<float>(data[start + 1]) / 255.0f;
    color.z = static_cast<float>(data[start + 2]) / 255.0f;
    return color;
}