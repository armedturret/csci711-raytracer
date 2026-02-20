#include "sphere.h"

using namespace std;

Sphere::Sphere(Material* m, glm::vec3 center, float radius) :
    Object(m),
    center(center),
    radius(radius)
{
}

bool Sphere::intersect(Ray ray, RayIntersection& out) const
{
    // Assumes ray.direction is normalized so A = 1
    // Ray and the sphere should both be in world space

    // "technically" radius could be squashed in a transform, but I don't care

    float b = 2.0f * glm::dot(ray.direction, ray.origin - center);

    float c = glm::dot(ray.origin - center, ray.origin - center) - pow(radius, 2.0f);

    float quad_term = pow(b, 2.0f) - 4.0f * c;

    // does not intersect
    if (quad_term < 0.0f)
        return false;

    float omega[] = { (-b + sqrt(quad_term)) / 2.0f, (-b - sqrt(quad_term)) / 2.0f };

    // Both intersections behind origin
    if (omega[0] <= 0.0f && omega[1] <= 0.0f)
        return false;

    float small_omega = omega[0];
    if (small_omega <= 0.0f || (omega[1] > 0.0f && omega[1] < small_omega))
        small_omega = omega[1];

    // Placeholder for UV calculations, fill in later if needed
    out.uv = glm::vec2(0.0f);
    out.position = ray.origin + ray.direction * small_omega;
    out.normal = glm::normalize(out.position - center);
    out.distance = small_omega;
    return true;
}

AABB Sphere::createAABB()
{
    glm::vec3 min = center - glm::vec3(radius);
    glm::vec3 max = center + glm::vec3(radius);
    return { min, max };
}
