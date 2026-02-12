#pragma once

#include "object.h"
#include "mesh.h"
#include "light.h"

#include <vector>
#include <memory>

enum class Plane
{
    x,
    y,
    z
};

struct KdTreeNode
{
    // objects only set in leaf node to save memory
    bool isLeaf;
    std::vector<Object*> objects;

    // not leaf node stuff
    Plane partitionPlane;
    float partitionCoord;
    std::shared_ptr<KdTreeNode> front;
    std::shared_ptr<KdTreeNode> rear;
};

class World
{
public:
    void add(Object* o);

    void add(Mesh* m);

    void add(Light* l);

    void buildKdTree(int maxObjectsPerLeaf, int maxDepth);

    /// <summary>
    /// Returns the closest intersecting point in front of minDistance
    /// </summary>
    /// <param name="ray">The ray to cast</param>
    /// <param name="hit">Data from intersecting an object</param>
    /// <param name="terminateOnAnything">Returns immediately if ANYTHING valid hit. (Skips lighting)</param>
    /// <param name="minDistance">Minimum distance for a valid "hit"</param>
    /// <param name="maxDistance">Maximum distance for a valid "hit". Set to negative for inf.</param>
    /// <returns></returns>
    bool raycast(Ray ray,
        RayIntersection& hit,
        bool terminateOnAnything = false,
        float minDistance = 0.0f,
        float maxDistance = -1.0f) const;

private:
    std::shared_ptr<KdTreeNode> buildKdNode(int maxObjectsPerLeaf,
        int maxDepth,
        AABB bounds,
        const std::vector<Object*>& nodeObjects,
        int depth);

    std::shared_ptr<KdTreeNode> rootNode;
    std::vector<Object*> objects;
    std::vector<Light*> lights;
};