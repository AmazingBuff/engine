//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/2d/segment2d.h"

AMAZING_NAMESPACE_BEGIN

Segment2D::Segment2D(const Point2D& p1, const Point2D& p2) : m_start(p1), m_end(p2) {}

Vector2D Segment2D::direction() const
{
    return m_end - m_start;
}

Float Segment2D::length() const
{
    return direction().norm();
}

DirectionDetection Segment2D::detect_point_direction(const Point2D& p) const
{
    Vector2D segment_direction = m_end - m_start;
    Vector2D point_direction = p - m_end;

    Float value = cross(segment_direction, point_direction);
    // left hand coordinate
    if (value > Max_Allowable_Error)
        return DirectionDetection::e_right;
    else if (value < -Max_Allowable_Error)
        return DirectionDetection::e_left;
    else
        return DirectionDetection::e_collinear;
}

AMAZING_NAMESPACE_END