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
    BVHNode* parent;
    BVHNode* left;
    BVHNode* right;

    // primitive has value when it is a leaf node
    Primitive* val;
};

struct BVHKeyCompare
{
    bool operator()(const Primitive* l, const Primitive* r) const
    {
        return true;
    }
};

INTERNAL_NAMESPACE_END
// todo: adjust bvh
class BVHTree final : public Internal::BinaryTree<Internal::TreeTrait<Internal::SetTrait<Primitive*, Internal::BVHKeyCompare, Allocator, false>, Internal::BVHNode>>
{
    using Tree = BinaryTree;

    using Tree::key_type;
    using Tree::value_type;
    using Tree::key_compare;
    using Tree::value_compare;
    using Tree::node_type;
    using Tree::allocator;
public:
    BVHTree() = default;
    ~BVHTree() override = default;
private:
    node_type* allocate_node(value_type&& val, node_type* parent) override;
    void insert_adjustment(node_type* node) override;
    void erase_adjustment(node_type* node) override;
};


AMAZING_NAMESPACE_END

#endif //CONSTRUCT_H
