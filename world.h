#pragma once

#include "object.h"

#include <vector>

struct RaycastHit
{
    Object* hit;
    glm::vec3 normal;
    glm::vec3 position;
};

class World
{
public:
    void add(Object* o);

    /// <summary>
    /// Returns the closest intersecting point in front of minDistance
    /// </summary>
    /// <param name="ray"></param>
    /// <param name="hit"></param>
    /// <param name="minDistance"></param>
    /// <returns></returns>
    bool raycast(Ray ray, RaycastHit& hit, float minDistance = 0.0f) const;

private:
    std::vector<Object*> objects;
};