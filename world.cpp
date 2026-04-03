#include "world.h"

#include <iostream>
#include <chrono>

using namespace std;

const int MAX_ILLUMINATE_DEPTH = 4;

bool isObjectInFrontOfPCoord(Object* o, float pCoord, int pAxis)
{
    const AABB& objectBox = o->getAABB();
    return objectBox.max[pAxis] > pCoord;
}

bool isObjectInRearOfPCoord(Object* o, float pCoord, int pAxis)
{
    const AABB& objectBox = o->getAABB();
    return objectBox.min[pAxis] <= pCoord;
}

World::World(glm::vec3 bgColor, 
    float shadowBias, 
    float worldBoundsBias) :
    bgColor(bgColor),
    shadowBias(shadowBias),
    worldBoundsBias(worldBoundsBias),
    worldBounds()
{
}

void World::add(Object* o)
{
    objects.push_back(o);
}

void World::add(Mesh* m)
{
    // this is stupid since mesh could fall out of scope, but I don't care
    for (int i = 0; i < m->getTriangles().size(); i++) {
        objects.push_back(m->getTriangles()[i].get());
    }
}

void World::add(Light* l)
{
    lights.push_back(l);
}

void World::buildKdTree(int maxObjectsPerLeaf, int maxDepth)
{
    cout << "Building K-D Tree..." << endl;
    auto startTime = chrono::system_clock::now();

    if (objects.size() == 0)
    {
        rootNode = make_shared<KdTreeNode<Object>>();
        rootNode->objects = objects;
        rootNode->isLeaf = true;
    }
    else
    {
        // find the bounds of the entire scene
        worldBounds = objects[0]->getAABB();
        for (auto o : objects)
        {
            const AABB& box = o->getAABB();

            worldBounds.min.x = glm::min(box.min.x, worldBounds.min.x);
            worldBounds.min.y = glm::min(box.min.y, worldBounds.min.y);
            worldBounds.min.z = glm::min(box.min.z, worldBounds.min.z);

            worldBounds.max.x = glm::max(box.max.x, worldBounds.max.x);
            worldBounds.max.y = glm::max(box.max.y, worldBounds.max.y);
            worldBounds.max.z = glm::max(box.max.z, worldBounds.max.z);
        }

        // slightly grow the world bounds to prevent floating point shenanigans
        worldBounds.min -= glm::vec3(worldBoundsBias);
        worldBounds.max += glm::vec3(worldBoundsBias);

        // build the node
        rootNode = buildKdNode<Object>(maxObjectsPerLeaf,
            maxDepth,
            worldBounds,
            objects,
            1,
            isObjectInFrontOfPCoord,
            isObjectInRearOfPCoord);
    }

    auto endTime = chrono::system_clock::now();
    chrono::duration<float> buildTime = endTime - startTime;
    cout << "K-D Tree built in " << buildTime.count() << " seconds!" << endl;
}

glm::vec3 World::illuminate(Ray ray,
    float minDistance,
    float maxDistance,
    int depth) const
{
    RayIntersection hit;
    Object* o = raycast(ray, hit, minDistance, maxDistance);
    if (o != nullptr)
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
            if (!raycast(lightRay, intersection, shadowBias, glm::distance(l->position, lightRay.origin)))
            {
                hit.visibleLights.push_back(l);
            }
        }

        o->material->illuminate(&hit);

        if (depth < MAX_ILLUMINATE_DEPTH)
        {
            if (o->material->kr > 0.0f)
            {
                Ray reflectedRay;
                reflectedRay.origin = hit.position;
                reflectedRay.direction = glm::normalize(glm::reflect(ray.direction, hit.normal));
                hit.irradiance += o->material->kr * illuminate(reflectedRay, shadowBias, -1.0f, depth + 1);
            }
        }
    }
    else
    {
        hit.irradiance = bgColor;
    }

    return hit.irradiance;
}

Object* World::raycast(Ray ray, RayIntersection& hit, float minDistance, float maxDistance) const
{
    if (!rootNode)
    {
        cout << "K-D Tree not built yet! Aborting raycast..." << endl;
        return nullptr;
    }

    // shortcut instead of actually using min distance :)
    ray.origin += ray.direction * minDistance;

    hit.distance = 0.0f;
    hit.incoming = ray.direction;

    // check if in world bounds
    float tmin, tmax;
    if (!worldBounds.intersects(ray, tmin, tmax))
    {
        return nullptr;
    }

    // we can do max distance by clamping far intersect!!!!
    if (maxDistance >= 0.0f)
        tmax = glm::min(maxDistance, tmax);
    glm::vec3 nearIntersect = ray.origin + ray.direction * tmin;
    glm::vec3 farIntersect = ray.origin + ray.direction * tmax;

    Object* object = rayTraverse(rootNode, nearIntersect, farIntersect, hit, ray);

    // need to add back in min distance
    hit.distance += minDistance;

    return object;
}

Object* World::rayTraverse(const std::shared_ptr<KdTreeNode<Object>>& node,
    const glm::vec3& nearInt,
    const glm::vec3& farInt,
    RayIntersection& hit,
    const Ray& ray) const
{
    if (node->isLeaf)
    {
        Object* closestObject = nullptr;

        for (auto o : node->objects)
        {
            // any intersections not between near and far should get thrown out
            RayIntersection intersection;
            if (o->intersect(ray, intersection) &&
                inRange(intersection.position[node->pAxis], nearInt[node->pAxis], farInt[node->pAxis]))
            {
                if (closestObject == nullptr || intersection.distance < hit.distance)
                {
                    hit.distance = intersection.distance;
                    hit.normal = intersection.normal;
                    hit.position = intersection.position;
                    hit.uv = intersection.uv;
                    closestObject = o;
                }
            }
        }

        return closestObject;
    }

    if (nearInt[node->pAxis] <= node->pCoord)
    {
        if (farInt[node->pAxis] <= node->pCoord)
        {
            // visit left node (aka rear)
            return rayTraverse(node->rear, nearInt, farInt, hit, ray);
        }
        else
        {
            // calculate intersection location
            float t = (node->pCoord - ray.origin[node->pAxis]) / ray.direction[node->pAxis];
            glm::vec3 middleInt = t * ray.direction + ray.origin;
            // visit left then right node
            Object* left = rayTraverse(node->rear, nearInt, middleInt, hit, ray);
            if (left)
                return left;
            return rayTraverse(node->front, middleInt, farInt, hit, ray);
        }
    }
    else if (farInt[node->pAxis] > node->pCoord)
    {
        // visit right node (aka front)
        return rayTraverse(node->front, nearInt, farInt, hit, ray);
    }
    else
    {
        // calculate intersection location
        float t = (node->pCoord - ray.origin[node->pAxis]) / ray.direction[node->pAxis];
        glm::vec3 middleInt = t * ray.direction + ray.origin;
        // visit right then left node
        Object* right = rayTraverse(node->front, nearInt, middleInt, hit, ray);
        if (right)
            return right;
        return rayTraverse(node->rear, middleInt, farInt, hit, ray);
    }
}

bool World::inRange(const float& t, const float& a, const float& b) const
{
    return t >= glm::min(a, b) && t <= glm::max(a, b);
}