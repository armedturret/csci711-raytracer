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

void PhongMaterial::illuminate(RayIntersection* hit)
{
    hit->irradiance = glm::vec3(0.0f);

    for (auto l : hit->visibleLights)
    {
        // Diffuse
        glm::vec3 lightDir = glm::normalize(l->position - hit->position);
        glm::vec3 diff = l->color * getDiffuseColor(hit) * glm::max(glm::dot(lightDir, hit->normal), 0.0f);
        hit->irradiance += kd * diff;

        // Specular
        // Reflect except ray inbound to point, returns ray outbound
        glm::vec3 reflectDir = glm::reflect(-lightDir, hit->normal);
        glm::vec3 view = -hit->incoming;
        glm::vec3 spec = l->color * colorSpecular * glm::pow(glm::max(glm::dot(reflectDir, view), 0.0f), ke);
        if (glm::dot(lightDir, hit->normal) <= 0.0f)
            spec = glm::vec3(0.0f);
        hit->irradiance += ks * spec;
    }
}