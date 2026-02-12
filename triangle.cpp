#include "triangle.h"

Triangle::Triangle(Material* m, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2):
    Object(m),
    p0(p0),
    p1(p1),
    p2(p2)
{}

bool Triangle::intersect(Ray ray, RayIntersection& out) const
{
    auto e1 = p1 - p0;
    auto e2 = p2 - p0;
    auto T = ray.origin - p0;
    auto P = glm::cross(ray.direction, e2);
    auto Q = glm::cross(T, e1);

    if (glm::dot(P, e1) == 0.0f)
    {
        // ray parallel to tri
        return false;
    }

    auto scalar = 1.0f / glm::dot(P, e1);
    auto omega = scalar * glm::dot(Q, e2);
    auto u = scalar * glm::dot(P, T);
    auto v = scalar * glm::dot(Q, ray.direction);

    if (omega < 0.0f || u < 0.0f || v < 0.0f || u + v > 1.0f)
    {
        // outside triangle or behind origin
        return false;
    }

    out.distance = omega;
    out.position = ray.origin + omega * ray.direction;
    out.normal = glm::normalize(glm::cross(e1, e2));
    return true;
}

AABB Triangle::createAABB()
{
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);

    min.x = glm::min(glm::min(p0.x, p1.x), p2.x);
    min.y = glm::min(glm::min(p0.y, p1.y), p2.y);
    min.z = glm::min(glm::min(p0.z, p1.z), p2.z);

    max.x = glm::max(glm::max(p0.x, p1.x), p2.x);
    max.y = glm::max(glm::max(p0.y, p1.y), p2.y);
    max.z = glm::max(glm::max(p0.z, p1.z), p2.z);

    return { min, max };
}
