//
// Created by AmazingBuff on 2025/6/23.
//

#include "geometry/algorithm/boolean.h"
#include "core/math/misc.h"

AMAZING_NAMESPACE_BEGIN

bool AABB_union(AABB const& lhs, AABB const& rhs, AABB& r)
{
    r.min = std::min(lhs.min, rhs.min);
    r.max = std::max(lhs.max, rhs.max);
    return true;
}

bool AABB_intersection(AABB const& lhs, AABB const& rhs, AABB& r)
{
    const Interval<Float> lx(lhs.min.x(), lhs.max.x());
    const Interval<Float> rx(rhs.min.x(), rhs.max.x());
    const Interval<Float> ly(lhs.min.y(), lhs.max.y());
    const Interval<Float> ry(rhs.min.y(), rhs.max.y());
    const Interval<Float> lz(lhs.min.z(), lhs.max.z());
    const Interval<Float> rz(rhs.min.z(), rhs.max.z());

    Interval<Float> x, y, z;
    if (intersect(lx, rx, x) && intersect(ly, ry, y) && intersect(rz, lz, z))
    {
        r = {{x.min(), y.min(), z.min()}, {x.max(), y.max(), z.max()}};
        return true;
    }
    return false;
}


AMAZING_NAMESPACE_END