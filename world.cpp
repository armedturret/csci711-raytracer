#include "world.h"

void World::add(Object* o)
{
    objects.push_back(o);
}

bool World::raycast(Ray ray, RaycastHit& hit, float minDistance) const
{
    bool foundObject = false;
    float smallestDistance = 0.0f;

    for (auto o : objects)
    {
        RayIntersection intersection;
        if (o->intersect(ray, intersection) && intersection.distance >= minDistance)
        {
            if (!foundObject || intersection.distance < smallestDistance)
            {
                foundObject = true;
                smallestDistance = intersection.distance;
                hit.hit = o;
                hit.normal = intersection.normal;
                hit.position = intersection.position;
            }
        }
    }

    return foundObject;
}