//
// Created by AmazingBuff on 2025/6/18.
//

#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include "geometry/scene.h"

AMAZING_NAMESPACE_BEGIN

// here we use avl to construct a bvh tree
struct BVHNode
{
    AABB aabb;
    BVHNode* parent;
    BVHNode* left;
    BVHNode* right;

    Primitive* primitive;
};

class BVHTree
{
public:

private:
    BVHNode* m_nodes;
    uint32_t m_node_count;
};


AMAZING_NAMESPACE_END

#endif //CONSTRUCT_H
