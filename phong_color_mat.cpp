#include "phong_color_mat.h"

PhongColorMaterial::PhongColorMaterial(glm::vec3 diffuse,
    glm::vec3 specular,
    float kd,
    float ks,
    float ke,
    float kr,
    float kt) :
    PhongMaterial(specular, kd, ks, ke, kr, kt),
    diffuseColor(diffuse)
{
}

glm::vec3 PhongColorMaterial::getDiffuseColor(const RayIntersection& hit)
{
    return diffuseColor;
}