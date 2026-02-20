#pragma once

#include "material.h"

class PhongMaterial : public Material
{
public:
    PhongMaterial(glm::vec3 specular, float kd, float ks, float ke);

    void illuminate(RayIntersection* hit) override;

    virtual glm::vec3 getDiffuseColor(RayIntersection* hit) = 0;

    // Colors are from 0-1 in each channel
    glm::vec3 colorSpecular;

    // Diffuse and specular constants (no ambient)
    float kd;
    float ks;

    // Specular exponent
    float ke;
};