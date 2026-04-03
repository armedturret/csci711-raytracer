#pragma once

#include "material.h"

class PhongMaterial : public Material
{
public:
    PhongMaterial(glm::vec3 specular,
        float kd,
        float ks,
        float ke,
        float kr,
        float kt);

    void illuminate(RayIntersection* hit) override;

    glm::vec3 getSpecularCoefficients(RayIntersection* hit) override;

    // Colors are from 0-1 in each channel
    glm::vec3 colorSpecular;

    // diffuse and specular coefficients (phong only)
    float kd;
    float ks;

    // Specular exponent
    float ke;
};