#pragma once

#include "object.h"
#include "vertex.h"

class Triangle : public Object
{
public:
    Triangle(Material* m, const Vertex& p0, const Vertex& p1, const Vertex& p2);

    bool intersect(Ray ray, RayIntersection& out) const override;

protected:
    AABB createAABB() override;

private:
    Vertex p0;
    Vertex p1;
    Vertex p2;
};