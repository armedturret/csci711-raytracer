#pragma once

#include "phong_mat.h"

class CheckerMaterial : public PhongMaterial
{
public:
    CheckerMaterial(glm::vec3 color1, glm::vec3 color2, float checkerSize, glm::vec3 specular, float kd, float ks, float ke);

    glm::vec3 getDiffuseColor(RayIntersection* hit) override;

    glm::vec3 color1;
    glm::vec3 color2;
    float checkerSize;
};