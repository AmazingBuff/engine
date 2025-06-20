//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/3d/primitive/cuboid.h"

AMAZING_NAMESPACE_BEGIN

Cuboid::Cuboid(Float x, Float y, Float z, Float width, Float height, Float depth)
    : m_min(x, y, z), m_max(x + width, y + height, z + depth) {}

Cuboid::Cuboid(const Point3D& point, Float width, Float height, Float depth)
    : m_min(point), m_max(point.x() + width, point.y() + height, point.z() + depth) {}

Cuboid::Cuboid(const Point3D& p_min, const Point3D& p_max) : m_min(p_min), m_max(p_max) {}

PrimitiveType Cuboid::type() const
{
    return PrimitiveType::e_cuboid;
}

Point3D Cuboid::min() const
{
    return m_min;
}

Point3D Cuboid::max() const
{
    return m_max;
}

AABB Cuboid::aabb() const
{
    return {m_min, m_max};
}

DirectionDetection Cuboid::detect_point_direction(const Point3D& point) const
{
    DirectionDetection d_x = in_interval(point.x(), m_min.x(), m_max.x());
    DirectionDetection d_y = in_interval(point.y(), m_min.y(), m_max.y());
    DirectionDetection d_z = in_interval(point.z(), m_min.z(), m_max.z());

    if (d_x == DirectionDetection::e_inner && d_y == DirectionDetection::e_inner && d_z == DirectionDetection::e_inner)
        return DirectionDetection::e_inner;
    else if (d_x == DirectionDetection::e_outer || d_y == DirectionDetection::e_outer || d_z == DirectionDetection::e_outer)
        return DirectionDetection::e_outer;
    else
        return DirectionDetection::e_border;
}


AMAZING_NAMESPACE_END