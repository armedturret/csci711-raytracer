#include "mesh.h"

Mesh::Mesh(Material* m, std::vector<glm::vec3> points, glm::mat4 modelT) :
    Object(m),
    points(points),
    modelT(modelT)
{
    for (int i = 0; i < points.size(); i++)
    {
        // pre-bake transform so it's faster
        this->points[i] = modelT * glm::vec4(points[i], 1.0f);
    }
}

bool Mesh::intersect(Ray ray, RayIntersection& out) const
{
    bool foundIntersection = false;
    for (int i = 0; i < points.size(); i += 3)
    {
        glm::vec3 p0 = points[i];
        glm::vec3 p1 = points[i + 1];
        glm::vec3 p2 = points[i + 2];

        auto e1 = p1 - p0;
        auto e2 = p2 - p0;
        auto T = ray.origin - p0;
        auto P = glm::cross(ray.direction, e2);
        auto Q = glm::cross(T, e1);

        if (glm::dot(P, e1) == 0.0f)
        {
            // ray parallel to tri
            continue;
        }

        auto scalar = 1.0f / glm::dot(P, e1);
        auto omega = scalar * glm::dot(Q, e2);
        auto u = scalar * glm::dot(P, T);
        auto v = scalar * glm::dot(Q, ray.direction);

        if (omega < 0.0f || u < 0.0f || v < 0.0f || u + v > 1.0f)
        {
            // outside triangle or behind origin
            continue;
        }

        if (!foundIntersection || omega < out.distance)
        {
            foundIntersection = true;
            out.distance = omega;
            out.position = ray.origin + omega * ray.direction;
            out.normal = glm::cross(e1, e2);
        }
    }

    return foundIntersection;
}