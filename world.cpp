#include "world.h"

#include <iostream>
#include <chrono>

using namespace std;

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
        rootNode = make_shared<KdTreeNode>();
        rootNode->objects = objects;
        rootNode->isLeaf = true;
    }
    else
    {
        // find the bounds of the entire scene
        AABB bounds = objects[0]->getAABB();
        for (auto o : objects)
        {
            const AABB& box = o->getAABB();

            bounds.min.x = glm::min(box.min.x, bounds.min.x);
            bounds.min.y = glm::min(box.min.y, bounds.min.y);
            bounds.min.z = glm::min(box.min.z, bounds.min.z);

            bounds.max.x = glm::max(box.max.x, bounds.max.x);
            bounds.max.y = glm::max(box.max.y, bounds.max.y);
            bounds.max.z = glm::max(box.max.z, bounds.max.z);
        }

        // build the node
        rootNode = buildKdNode(maxObjectsPerLeaf,
            maxDepth,
            bounds,
            objects,
            1);
    }

    auto endTime = chrono::system_clock::now();
    chrono::duration<float> buildTime = endTime - startTime;
    cout << "K-D Tree built in " << buildTime.count() << " seconds!" << endl;
}

bool World::raycast(Ray ray, RayIntersection& hit, bool terminateOnAnything, float minDistance, float maxDistance) const
{
    if (!rootNode)
    {
        cout << "K-D Tree not built yet! Aborting raycast..." << endl;
        return false;
    }

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

std::shared_ptr<KdTreeNode> World::buildKdNode(int maxObjectsPerLeaf,
    int maxDepth,
    AABB bounds,
    const std::vector<Object*>& nodeObjects,
    int depth)
{
    if (nodeObjects.size() <= maxObjectsPerLeaf || depth >= maxDepth)
    {
        auto leafNode = make_shared<KdTreeNode>();
        if (nodeObjects.size() >= 2 * maxObjectsPerLeaf)
        {
            cout << "Large leaf warning: Making leaf node with " << nodeObjects.size() << " objects!" << endl;
        }
        leafNode->isLeaf = true;
        for (auto o : nodeObjects)
        {
            leafNode->objects.push_back(o);
        }
        return leafNode;
    }

    auto node = make_shared<KdTreeNode>();

    AABB frontBounds = bounds;
    AABB rearBounds = bounds;

    // naive partitioning
    switch (depth % 3)
    {
    case 0:
        node->partitionPlane = Plane::x;
        node->partitionCoord = (bounds.max.x + bounds.min.x) / 2.0f;
        frontBounds.min.x = node->partitionCoord;
        rearBounds.max.x = node->partitionCoord;
        break;
    case 1:
        node->partitionPlane = Plane::y;
        node->partitionCoord = (bounds.max.y + bounds.min.y) / 2.0f;
        frontBounds.min.y = node->partitionCoord;
        rearBounds.max.y = node->partitionCoord;
        break;
    case 2:
        node->partitionPlane = Plane::z;
        node->partitionCoord = (bounds.max.z + bounds.min.z) / 2.0f;
        frontBounds.min.z = node->partitionCoord;
        rearBounds.max.z = node->partitionCoord;
        break;
    }

    vector<Object*> frontObjects;
    vector<Object*> rearObjects;

    for (auto o : nodeObjects)
    {
        // Objects can be in both front and rear!
        const AABB& objectBox = o->getAABB();
        switch (node->partitionPlane)
        {
        case Plane::x:
            if (objectBox.max.x > node->partitionCoord)
                frontObjects.push_back(o);
            if (objectBox.min.x < node->partitionCoord)
                rearObjects.push_back(o);
            break;
        case Plane::y:
            if (objectBox.max.y > node->partitionCoord)
                frontObjects.push_back(o);
            if (objectBox.min.y < node->partitionCoord)
                rearObjects.push_back(o);
            break;
        case Plane::z:
            if (objectBox.max.z > node->partitionCoord)
                frontObjects.push_back(o);
            if (objectBox.min.z < node->partitionCoord)
                rearObjects.push_back(o);
            break;
        }
    }

    node->front = buildKdNode(maxObjectsPerLeaf, maxDepth, frontBounds, frontObjects, depth + 1);
    node->rear = buildKdNode(maxObjectsPerLeaf, maxDepth, rearBounds, rearObjects, depth + 1);

    return node;
}