//
// Created by AmazingBuff on 2025/6/18.
//

#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include "geometry/scene.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

struct BVHNode
{
    AABB aabb;
    BVHNode* left;
    BVHNode* right;

    // primitive has value when it is a leaf node
    Primitive* val;
};

INTERNAL_NAMESPACE_END

// a static bvh builder, us SAH algorithm
class StaticBVHBuilder
{
public:
    explicit StaticBVHBuilder(Vector<Primitive*> const& primitives);
    ~StaticBVHBuilder();
private:
    Internal::BVHNode* m_root;
};


AMAZING_NAMESPACE_END

#endif //CONSTRUCT_H
