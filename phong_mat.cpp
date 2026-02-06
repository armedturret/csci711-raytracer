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
    // TODO: implement phong
    hit->irradiance = color;
}
