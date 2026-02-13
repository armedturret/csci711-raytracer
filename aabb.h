#pragma once

#include <glm/glm.hpp>

#include "ray.h"

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;

    // Returns true if the ray intersects, and sets the intersection distances if so
    bool intersects(const Ray& ray, float& tmin, float& tmax) const
    {
        // Taken from: https://tavianator.com/2011/ray_box.html
        tmin = -INFINITY, tmax = INFINITY;

        if (ray.direction.x != 0.0f)
        {
            float tx1 = (min.x - ray.origin.x) / ray.direction.x;
            float tx2 = (max.x - ray.origin.x) / ray.direction.x;

            tmin = glm::max(tmin, glm::min(tx1, tx2));
            tmax = glm::min(tmax, glm::max(tx1, tx2));
        }

        if (ray.direction.y != 0.0f)
        {
            float ty1 = (min.y - ray.origin.y) / ray.direction.y;
            float ty2 = (max.y - ray.origin.y) / ray.direction.y;

            tmin = glm::max(tmin, glm::min(ty1, ty2));
            tmax = glm::min(tmax, glm::max(ty1, ty2));
        }

        if (ray.direction.z != 0.0f)
        {
            float tz1 = (min.z - ray.origin.z) / ray.direction.z;
            float tz2 = (max.z - ray.origin.z) / ray.direction.z;

            tmin = glm::max(tmin, glm::min(tz1, tz2));
            tmax = glm::min(tmax, glm::max(tz1, tz2));
        }

        return tmax >= tmin;
    }
};