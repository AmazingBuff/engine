//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/2d/convex_hull/triangle2d.h"
#include "geometry/2d/segment2d.h"

AMAZING_NAMESPACE_BEGIN

Triangle2D::Triangle2D(const Point2D& p0, const Point2D& p1, const Point2D& p2)
    : m_vertices{p0, p1, p2} {}




AMAZING_NAMESPACE_END