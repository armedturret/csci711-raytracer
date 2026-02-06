#include "world.h"

void World::add(Object* o)
{
    objects.push_back(o);
}

void World::add(Light* l)
{
    lights.push_back(l);
}

bool World::raycast(Ray ray, RayIntersection& hit, bool terminateOnAnything, float minDistance, float maxDistance) const
{
    hit.distance = 0.0f;
    hit.incoming = ray.direction;
    Object* closestObject = nullptr;

    for (auto o : objects)
    {
        RayIntersection intersection;
        if (o->intersect(ray, intersection) &&
            intersection.distance >= minDistance &&
            (intersection.distance <= maxDistance || maxDistance <= 0.0f))
        {
            if (closestObject == nullptr || intersection.distance < hit.distance)
            {
                hit.distance = intersection.distance;
                hit.normal = intersection.normal;
                hit.position = intersection.position;
                closestObject = o;

                if (terminateOnAnything)
                    return true;
            }
        }
    }

    if (!terminateOnAnything && closestObject)
    {
        Ray lightRay;
        lightRay.origin = hit.position;
        // Need to calculate visible lights first
        for (auto l : lights)
        {
            lightRay.direction = glm::normalize(l->position - hit.position);

            RayIntersection intersection;
            // Cast a ray from intersection point to a light and see if anything in between
            // Min distance is non-zero to prevent self intersection
            if (!raycast(lightRay, intersection, true, 0.00001f, glm::distance(l->position, lightRay.origin)))
            {
                hit.visibleLights.push_back(l);
            }
        }

        closestObject->material->illuminate(&hit);
    }

    return closestObject != nullptr;
}