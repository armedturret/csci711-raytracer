#pragma once

#include <glm/glm.hpp>

struct RayIntersection;

class Material
{
public:
    // Calculates only the diffuse component of illumination
    virtual glm::vec3 illuminateDiffuse(const RayIntersection& hit, glm::vec3 lightIncidentDir, glm::vec3 power) = 0;

    // Calculates only the specular component of illumination
    virtual glm::vec3 illuminateSpecular(const RayIntersection& hit, glm::vec3 lightIncidentDir, glm::vec3 power) = 0;

    // diffuse and specular components by channel (necessary for emission step of photon mapping)
    // these do not need lighting data
    virtual glm::vec3 getDiffuseCoefficients(const RayIntersection& hit) = 0;
    virtual glm::vec3 getSpecularCoefficients(const RayIntersection& hit) = 0;

    // reflective component
    float kr = 0.0f;

    // transmissive component
    float kt = 0.0f;
};