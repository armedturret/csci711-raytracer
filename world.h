#pragma once

#include "object.h"
#include "mesh.h"
#include "light.h"

#include <vector>
#include <memory>

struct KdTreeNode
{
    // objects only set in leaf node to save memory
    bool isLeaf;
    std::vector<Object*> objects;

    // not leaf node stuff
    // 0 = x, 1 = y, 2 = z (easier to index)
    int pAxis;
    float pCoord;
    std::shared_ptr<KdTreeNode> front;
    std::shared_ptr<KdTreeNode> rear;
};

class World
{
public:
    World(glm::vec3 bgColor);

    void add(Object* o);

    void add(Mesh* m);

    void add(Light* l);

    void buildKdTree(int maxObjectsPerLeaf, int maxDepth);

    /// <summary>
    /// Returns the closest intersecting point in front of minDistance
    /// </summary>
    /// <param name="ray">The ray to cast</param>
    /// <param name="hit">Data from intersecting an object</param>
    /// <param name="doLighting">Should lighting be calculated?</param>
    /// <param name="minDistance">Minimum distance for a valid "hit"</param>
    /// <param name="maxDistance">Maximum distance for a valid "hit". Set to negative for inf.</param>
    /// <returns></returns>
    bool raycast(Ray ray,
        RayIntersection& hit,
        bool doLighting = false,
        float minDistance = 0.0f,
        float maxDistance = -1.0f) const;

private:
    std::shared_ptr<KdTreeNode> buildKdNode(const int& maxObjectsPerLeaf,
        const int& maxDepth,
        const AABB& bounds,
        const std::vector<Object*>& nodeObjects,
        const int& depth);

    Object* rayTraverse(const std::shared_ptr<KdTreeNode>& node,
        const glm::vec3& nearInt,
        const glm::vec3& farInt,
        RayIntersection& hit,
        const Ray& ray) const;

    // is t between a and b?
    bool inRange(const float& t, const float& a, const float& b) const;

    AABB worldBounds;
    std::shared_ptr<KdTreeNode> rootNode;
    std::vector<Object*> objects;
    std::vector<Light*> lights;

    glm::vec3 bgColor;
};