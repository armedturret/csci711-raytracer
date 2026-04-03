#include "checker_mat.h"

#include "ray.h"

CheckerMaterial::CheckerMaterial(glm::vec3 color1,
    glm::vec3 color2,
    float checkerSize,
    glm::vec3 specular,
    float kd,
    float ks,
    float ke,
    float kr,
    float kt) :
    PhongMaterial(specular, kd, ks, ke, kr, kt),
    color1(color1),
    color2(color2),
    checkerSize(checkerSize)
{
}

glm::vec3 CheckerMaterial::getDiffuseCoefficients(RayIntersection* hit)
{
    if (static_cast<int>(hit->uv.x / checkerSize) % 2 == static_cast<int>(hit->uv.y / checkerSize) % 2)
        return color1;
    else
        return color2;
}