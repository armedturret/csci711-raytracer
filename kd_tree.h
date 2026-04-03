#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <functional>

using namespace std;

template <typename T> struct KdTreeNode
{
    // items only set in leaf node to save memory
    bool isLeaf;
    vector<T> objects;

    // not leaf node stuff
    // 0 = x, 1 = y, 2 = z (easier to index)
    int pAxis;
    float pCoord;
    shared_ptr<KdTreeNode<T>> front;
    shared_ptr<KdTreeNode<T>> rear;
};

// Partitions a list of objects using a lambda function
// Objects can be points or bounding boxes, so the partition function will differ
// Uses naive partitioning, splitting down the middle
template <typename T> shared_ptr<KdTreeNode<T>> buildKdNode(const int& maxObjectsPerLeaf,
    const int& maxDepth,
    const AABB& bounds,
    const vector<T>& nodeObjects,
    const int& depth,
    const function<bool(T, float, int)>& inFront,
    const function<bool(T, float, int)>& inRear,
    const function<float(const vector<T>&, const AABB&, int)>& makePartition)
{
    if (nodeObjects.size() <= maxObjectsPerLeaf || depth >= maxDepth)
    {
        auto leafNode = make_shared<KdTreeNode<T>>();
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

    auto node = make_shared<KdTreeNode<T>>();

    AABB frontBounds = bounds;
    AABB rearBounds = bounds;

    // naive partitioning
    node->pAxis = depth % 3;
    node->pCoord = makePartition(nodeObjects, bounds, node->pAxis);
    frontBounds.min[node->pAxis] = node->pCoord;
    rearBounds.max[node->pAxis] = node->pCoord;

    vector<T> frontObjects;
    vector<T> rearObjects;

    for (auto o : nodeObjects)
    {
        // Objects can be in both front and rear!
        if(inFront(o, node->pCoord, node->pAxis))
            frontObjects.push_back(o);
        if(inRear(o, node->pCoord, node->pAxis))
            rearObjects.push_back(o);
    }

    node->front = buildKdNode<T>(maxObjectsPerLeaf, maxDepth, frontBounds, frontObjects, depth + 1, inFront, inRear, makePartition);
    node->rear = buildKdNode<T>(maxObjectsPerLeaf, maxDepth, rearBounds, rearObjects, depth + 1, inFront, inRear, makePartition);

    return node;
}