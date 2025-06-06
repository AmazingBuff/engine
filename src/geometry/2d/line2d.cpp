//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/2d/line2d.h"

AMAZING_NAMESPACE_BEGIN

Line2D::Line2D(const Point2D& o, const Vector2D& d) : m_origin(o), m_direction(d.normalized()) {}

DirectionDetection Line2D::detect_point_direction(const Point2D& p) const
{
    Vector2D point_direction = p - m_origin;

    Float value = cross(m_direction, point_direction);
    if (value > Max_Allowable_Error)
        return DirectionDetection::e_right;
    else if (value < -Max_Allowable_Error)
        return DirectionDetection::e_left;
    else
        return DirectionDetection::e_collinear;
}


AMAZING_NAMESPACE_END