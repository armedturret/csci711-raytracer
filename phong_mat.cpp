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

glm::vec3 PhongMaterial::illuminate(RayIntersection* hit, glm::vec3 incidentDir, glm::vec3 power)
{
    glm::vec3 irradiance = glm::vec3(0.0f);

    // Diffuse
    glm::vec3 diff = getDiffuseCoefficients(hit) * glm::max(glm::dot(-incidentDir, hit->normal), 0.0f);
    irradiance += kd * diff;

    // Specular
    // Reflect except ray inbound to point, returns ray outbound
    glm::vec3 reflectDir = glm::reflect(incidentDir, hit->normal);
    glm::vec3 view = -hit->incoming;
    glm::vec3 spec = getSpecularCoefficients(hit) * glm::pow(glm::max(glm::dot(reflectDir, view), 0.0f), ke);
    if (glm::dot(-incidentDir, hit->normal) <= 0.0f)
        spec = glm::vec3(0.0f);
    irradiance += ks * spec;

    return irradiance * power;
}

glm::vec3 PhongMaterial::getSpecularCoefficients(RayIntersection* hit)
{
    return colorSpecular;
}