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

    glm::vec3 illuminate(const RayIntersection& hit, glm::vec3 lightIncidentDir, glm::vec3 power) override;

    glm::vec3 getDiffuseCoefficients(const RayIntersection& hit) override;

    glm::vec3 getSpecularCoefficients(const RayIntersection& hit) override;

    // Grabs the diffuse color before it's been scaled by kd
    virtual glm::vec3 getDiffuseColor(const RayIntersection& hit) = 0;

    // Colors are from 0-1 in each channel
    glm::vec3 colorSpecular;

    // diffuse and specular coefficients (phong only)
    float kd;
    float ks;

    // Specular exponent
    float ke;
};