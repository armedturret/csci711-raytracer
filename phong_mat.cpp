#include "phong_mat.h"

#include "ray.h"

PhongMaterial::PhongMaterial(glm::vec3 color,
    glm::vec3 specular,
    float kd,
    float ks,
    float ke) :
    color(color),
    colorSpecular(specular),
    kd(kd),
    ks(ks),
    ke(ke)
{
}

void PhongMaterial::illuminate(RayIntersection* hit)
{
    hit->irradiance = glm::vec3(0.0f);

    for (auto l : hit->visibleLights)
    {
        // diffuse TODO: FIX
        glm::vec3 lightDir = glm::normalize(l->position - hit->position);
        hit->irradiance += kd * l->color * color * glm::dot(lightDir, hit->normal);

        // TODO: Specular
    }
}