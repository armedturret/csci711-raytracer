#include "phong_mat.h"

#include "ray.h"

PhongMaterial::PhongMaterial(glm::vec3 specular,
    float kd,
    float ks,
    float ke,
    float kr,
    float kt,
    float indexOfRefraction) :
    colorSpecular(specular),
    kd(kd),
    ks(ks),
    ke(ke)
{
    this->kr = kr;
    this->kt = kt;
    this->indexOfRefraction = indexOfRefraction;
}

glm::vec3 PhongMaterial::illuminateDiffuse(const RayIntersection& hit, glm::vec3 lightIncidentDir, glm::vec3 power)
{
    return getDiffuseCoefficients(hit) * glm::max(glm::dot(-lightIncidentDir, hit.normal), 0.0f) * power;
}

glm::vec3 PhongMaterial::illuminateSpecular(const RayIntersection& hit, glm::vec3 lightIncidentDir, glm::vec3 power)
{
    if (glm::dot(-lightIncidentDir, hit.normal) <= 0.0f)
        return glm::vec3(0.0f);

    glm::vec3 reflectDir = glm::reflect(lightIncidentDir, hit.normal);
    glm::vec3 view = -hit.incidentDir;
    glm::vec3 spec = getSpecularCoefficients(hit) * glm::pow(glm::max(glm::dot(reflectDir, view), 0.0f), ke);
    return spec * power;
}

glm::vec3 PhongMaterial::getDiffuseCoefficients(const RayIntersection& hit)
{
    return kd * getDiffuseColor(hit);
}

glm::vec3 PhongMaterial::getSpecularCoefficients(const RayIntersection& hit)
{
    return ks * colorSpecular;
}