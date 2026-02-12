#pragma once

#include <vector>

#include "material.h"
#include "ray.h"
#include "aabb.h"

class Object
{
public:
    Object(Material* material) : material(material),
        aabbCreated(false),
        aabb({ glm::vec3(0.0f), glm::vec3(0.0f) }) 
    {}

    /// <summary>
    /// Returns the closest (non-negative) intersection
    /// </summary>
    /// <param name="ray"></param>
    /// <param name="out"></param>
    /// <returns></returns>
    virtual bool intersect(Ray ray, RayIntersection& out) const = 0;

    const AABB& getAABB() { 
        if (!aabbCreated)
        {
            aabb = createAABB();
            aabbCreated = true;
        }
        return aabb; 
    }

    Material* material;

protected:
    virtual AABB createAABB() = 0;

private:
    bool aabbCreated;
    AABB aabb;
};