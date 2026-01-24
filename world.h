#pragma once

#include "object.h"

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

    bool raycast(Ray ray, RaycastHit& hit);
};