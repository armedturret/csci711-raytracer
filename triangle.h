#pragma once

#include "object.h"

class Triangle : public Object
{
public:
    Triangle(Material* m, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);

    bool intersect(Ray ray, RayIntersection& out) const override;

protected:
    AABB createAABB() override;

private:
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;
};