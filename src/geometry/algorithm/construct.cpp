//
// Created by AmazingBuff on 2025/6/18.
//

#include "geometry/algorithm/construct.h"
#include "geometry/algorithm/boolean.h"

AMAZING_NAMESPACE_BEGIN

using Internal::BVHNode;
static constexpr Float SAH_Traversal_Time = 0.5;
static constexpr Float SAH_Intersection_Time = 1.0;


static Float AABB_half_surface_area(AABB const& aabb)
{
    float dx = aabb.max.x() - aabb.min.x();
    float dy = aabb.max.y() - aabb.min.y();
    float dz = aabb.max.z() - aabb.min.z();
    return dx * dy + dy * dz + dz * dx;
}

static BVHNode* build_bvh(Vector<Primitive*>& primitives)
{
    BVHNode* node = PLACEMENT_NEW(BVHNode, sizeof(BVHNode));

    if (primitives.size() < 2)
    {
        node->left = nullptr;
        node->right = nullptr;
        node->aabb = primitives[0]->aabb();
        node->val = primitives[0];
        return node;
    }

    node->aabb = AABB(Point3D(std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max()),
        Point3D(std::numeric_limits<Float>::min(), std::numeric_limits<Float>::min(), std::numeric_limits<Float>::min()));

    uint32_t primitive_count = primitives.size();
    Vector<AABB> primitive_aabbs;
    primitive_aabbs.reserve(primitive_count);
    for (Primitive const* primitive : primitives)
    {
        AABB primitive_aabb = primitive->aabb();
        AABB_union(node->aabb, primitive_aabb, node->aabb);
        primitive_aabbs.emplace_back(primitive_aabb);
    }

    Float total_area = AABB_half_surface_area(node->aabb);
    Float best_cost = std::numeric_limits<Float>::max();
    uint32_t best_axis = 0;
    uint32_t best_split = 0;

    for (uint32_t axis = 0; axis < 3; ++axis)
    {
        sort(primitives, [axis](Primitive const* a, Primitive const* b)
        {
            if (axis == 0) return a->aabb().min.x() < b->aabb().min.x();
            if (axis == 1) return a->aabb().min.y() < b->aabb().min.y();
            return a->aabb().min.z() < b->aabb().min.z();
        });

        Vector<AABB> left(primitive_count - 1), right(primitive_count - 1);
        left[0] = primitive_aabbs[0];
        right[primitive_count - 2] = primitive_aabbs[primitive_count - 1];
        for (uint32_t i = 1; i < primitive_count - 1; ++i)
            AABB_union(left[i - 1], primitive_aabbs[i], left[i]);
        for (int i = primitive_count - 3; i >= 0; --i)
            AABB_union(right[i + 1], primitive_aabbs[i + 1], right[i]);

        for (uint32_t i = 0; i < primitive_count - 1; ++i)
        {
            AABB left_box = left[i];
            AABB right_box = right[i];

            Float cost = SAH_Traversal_Time + (AABB_half_surface_area(left_box) * (i + 1) + AABB_half_surface_area(right_box) * (primitive_count - 1 - i)) * SAH_Intersection_Time / total_area;

            if (cost < best_cost)
            {
                best_cost = cost;
                best_axis = axis;
                best_split = i;
            }
        }
    }

    if (best_axis != 2)
    {
        sort(primitives, [best_axis](Primitive const* a, Primitive const* b)
        {
            if (best_axis == 0) return a->aabb().min.x() < b->aabb().min.x();
            if (best_axis == 1) return a->aabb().min.y() < b->aabb().min.y();
            return a->aabb().min.z() < b->aabb().min.z();
        });
    }

    Vector<Primitive*> left_primitives(best_split + 1);
    Vector<Primitive*> right_primitives(primitive_count - best_split - 1);

    memcpy(left_primitives.data(), primitives.data(), (best_split + 1) * sizeof(Primitive*));
    memcpy(right_primitives.data(), primitives.data() + best_split + 1, (primitive_count - best_split - 1) * sizeof(Primitive*));

    node->left = build_bvh(left_primitives);
    node->right = build_bvh(right_primitives);

    return node;
}

static void release_bvh(BVHNode* node)
{
    if (node->left)
        release_bvh(node->left);
    if (node->right)
        release_bvh(node->right);
    PLACEMENT_DELETE(BVHNode, node);
}

StaticBVHBuilder::StaticBVHBuilder(Vector<Primitive*> const& primitives)
{
    Vector<Primitive*> primitives_copy(primitives.size());
    memcpy(primitives_copy.data(), primitives.data(), primitives.size() * sizeof(Primitive*));

    m_root = build_bvh(primitives_copy);
}

StaticBVHBuilder::~StaticBVHBuilder()
{
    release_bvh(m_root);
}


AMAZING_NAMESPACE_END