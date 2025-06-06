//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/2d/convex_hull/rectangle2d.h"

AMAZING_NAMESPACE_BEGIN

Rectangle2D::Rectangle2D(Float x, Float y, Float width, Float height)
    : m_min(std::min(x, x + width), std::min(y, y + height)),
      m_max(std::max(x, x + width), std::max(y, y + height)) {}

Rectangle2D::Rectangle2D(const Point2D& p_min, const Point2D& p_max)
    : m_min(p_min), m_max(p_max) {}



AMAZING_NAMESPACE_END