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
                    break;
            }
        }
    }

    if (!terminateOnAnything && closestObject)
    {
        // TODO: Calculate visible lights before calling this
        closestObject->material->illuminate(&hit);
    }

    return closestObject != nullptr;
}