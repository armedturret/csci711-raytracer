#pragma once

#include "phong_mat.h"

class PhongColorMaterial : public PhongMaterial
{
public:
    PhongColorMaterial(glm::vec3 diffuse, glm::vec3 specular, float kd, float ks, float ke);

    glm::vec3 getDiffuseColor(RayIntersection* hit) override;

private:
    glm::vec3 diffuseColor;
}; 