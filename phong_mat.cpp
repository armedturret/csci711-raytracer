#include "phong_mat.h"

#include "ray.h"

PhongMaterial::PhongMaterial(glm::vec3 specular,
    float kd,
    float ks,
    float ke,
    float kr,
    float kt) :
    colorSpecular(specular),
    kd(kd),
    ks(ks),
    ke(ke)
{
    this->kr = kr;
    this->kt = kt;
}

glm::vec3 PhongMaterial::illuminate(const RayIntersection& hit, glm::vec3 lightIncidentDir, glm::vec3 power)
{
    glm::vec3 irradiance = glm::vec3(0.0f);

    // Diffuse
    irradiance += illuminateDiffuse(hit, lightIncidentDir, power);

    // Specular
    // Reflect except ray inbound to point, returns ray outbound
    glm::vec3 reflectDir = glm::reflect(lightIncidentDir, hit.normal);
    glm::vec3 view = -hit.incidentDir;
    glm::vec3 spec = getSpecularCoefficients(hit) * glm::pow(glm::max(glm::dot(reflectDir, view), 0.0f), ke);
    if (glm::dot(-lightIncidentDir, hit.normal) <= 0.0f)
        spec = glm::vec3(0.0f);
    irradiance += spec * power;

    return irradiance;
}

glm::vec3 PhongMaterial::illuminateDiffuse(const RayIntersection& hit, glm::vec3 lightIncidentDir, glm::vec3 power)
{
    return getDiffuseCoefficients(hit) * glm::max(glm::dot(-lightIncidentDir, hit.normal), 0.0f) * power;
}

glm::vec3 PhongMaterial::getDiffuseCoefficients(const RayIntersection& hit)
{
    return kd * getDiffuseColor(hit);
}

glm::vec3 PhongMaterial::getSpecularCoefficients(const RayIntersection& hit)
{
    return ks * colorSpecular;
}