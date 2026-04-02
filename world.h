#pragma once

#include "object.h"
#include "mesh.h"
#include "light.h"
#include "kd_tree.h"

#include <vector>
#include <memory>

class World
{
public:
    World(glm::vec3 bgColor);

    void add(Object* o);

    void add(Mesh* m);

    void add(Light* l);

    void buildKdTree(int maxObjectsPerLeaf, int maxDepth);

    // Returns a ray's irradiance
    glm::vec3 illuminate(Ray ray,
        float minDistance = 0.0f,
        float maxDistance = -1.0f,
        int depth = 0) const;

    /// <summary>
    /// Returns the closest intersecting point in front of minDistance
    /// </summary>
    /// <param name="ray">The ray to cast</param>
    /// <param name="hit">Data from intersecting an object</param>
    /// <param name="doLighting">Should lighting be calculated?</param>
    /// <param name="minDistance">Minimum distance for a valid "hit"</param>
    /// <param name="maxDistance">Maximum distance for a valid "hit". Set to negative for inf.</param>
    /// <returns></returns>
    Object* raycast(Ray ray,
        RayIntersection& hit,
        float minDistance = 0.0f,
        float maxDistance = -1.0f) const;

private:
    Object* rayTraverse(const std::shared_ptr<KdTreeNode<Object>>& node,
        const glm::vec3& nearInt,
        const glm::vec3& farInt,
        RayIntersection& hit,
        const Ray& ray) const;

    // is t between a and b?
    bool inRange(const float& t, const float& a, const float& b) const;

    AABB worldBounds;
    std::shared_ptr<KdTreeNode<Object>> rootNode;
    std::vector<Object*> objects;
    std::vector<Light*> lights;

    glm::vec3 bgColor;
};