//
// Created by AmazingBuff on 2025/6/18.
//

#include "geometry/algorithm/construct.h"

AMAZING_NAMESPACE_BEGIN

static Float AABB_half_surface_area(AABB const& aabb)
{
    float dx = aabb.max.x() - aabb.min.x();
    float dy = aabb.max.y() - aabb.min.y();
    float dz = aabb.max.z() - aabb.min.z();
    return dx * dy + dy * dz + dz * dx;
}

BVHTree::node_type* BVHTree::allocate_node(value_type&& val, node_type* parent)
{
    node_type* node = allocator::allocate(1);
    node->parent = parent;
    node->left = nullptr;
    node->right = nullptr;
    node->val = val;
    node->aabb = node->val->aabb();

    return node;
}

void BVHTree::insert_adjustment(node_type* node)
{

}

void BVHTree::erase_adjustment(node_type* node)
{

}


AMAZING_NAMESPACE_END