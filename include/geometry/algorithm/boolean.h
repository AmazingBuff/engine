//
// Created by AmazingBuff on 2025/6/23.
//

#ifndef BOOLEAN_H
#define BOOLEAN_H

#include "geometry/geometry.h"

AMAZING_NAMESPACE_BEGIN

bool AABB_union(AABB const& lhs, AABB const& rhs, AABB& r);
bool AABB_intersection(AABB const& lhs, AABB const& rhs, AABB& r);

AMAZING_NAMESPACE_END
#endif //BOOLEAN_H
