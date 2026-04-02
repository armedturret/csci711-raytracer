#pragma once

#include <memory>
#include <vector>

template <typename T> struct KdTreeNode
{
    // items only set in leaf node to save memory
    bool isLeaf;
    std::vector<T*> items;

    // not leaf node stuff
    // 0 = x, 1 = y, 2 = z (easier to index)
    int pAxis;
    float pCoord;
    std::shared_ptr<KdTreeNode<T>> front;
    std::shared_ptr<KdTreeNode<T>> rear;
};