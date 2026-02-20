#include "triangle.h"

using namespace std;

Triangle::Triangle(Material* m, const Vertex& p0, const Vertex& p1, const Vertex& p2):
    Object(m),
    p0(p0),
    p1(p1),
    p2(p2)
{}

bool Triangle::intersect(Ray ray, RayIntersection& out) const
{
    auto e1 = p1.pos - p0.pos;
    auto e2 = p2.pos - p0.pos;
    auto T = ray.origin - p0.pos;
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
    auto w = u + v;

    if (omega < 0.0f || u < 0.0f || v < 0.0f || w > 1.0f)
    {
        // outside triangle or behind origin
        return false;
    }

    out.uv = glm::vec2(
        u * p0.uv.x + v * p1.uv.x + w * p2.uv.x,
        u * p0.uv.y + v * p1.uv.y + w * p2.uv.y
    );
    out.distance = omega;
    out.position = ray.origin + omega * ray.direction;
    out.normal = glm::normalize(glm::cross(e1, e2));
    return true;
}

AABB Triangle::createAABB()
{
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);

    min.x = glm::min(glm::min(p0.pos.x, p1.pos.x), p2.pos.x);
    min.y = glm::min(glm::min(p0.pos.y, p1.pos.y), p2.pos.y);
    min.z = glm::min(glm::min(p0.pos.z, p1.pos.z), p2.pos.z);

    max.x = glm::max(glm::max(p0.pos.x, p1.pos.x), p2.pos.x);
    max.y = glm::max(glm::max(p0.pos.y, p1.pos.y), p2.pos.y);
    max.z = glm::max(glm::max(p0.pos.z, p1.pos.z), p2.pos.z);

    return { min, max };
}
