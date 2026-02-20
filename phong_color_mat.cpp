#include "phong_color_mat.h"

PhongColorMaterial::PhongColorMaterial(glm::vec3 diffuse, glm::vec3 specular, float kd, float ks, float ke):
    PhongMaterial(specular, kd, ks, ke),
    diffuseColor(diffuse)
{
}

glm::vec3 PhongColorMaterial::getDiffuseColor(RayIntersection* hit)
{
    return diffuseColor;
}