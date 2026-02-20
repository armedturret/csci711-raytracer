#pragma once

#include <string>

#include "phong_mat.h"

class TextureMaterial : public PhongMaterial
{
public:
    TextureMaterial(const std::string& imgPath, glm::vec3 specular, float kd, float ks, float ke);
    ~TextureMaterial();

    glm::vec3 getDiffuseColor(RayIntersection* hit) override;

private:
    unsigned char* data;
    glm::ivec2 size;
    int channels;
};