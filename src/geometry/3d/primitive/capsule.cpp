//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/3d/primitive/capsule.h"
#include "geometry/3d/primitive/cuboid.h"

AMAZING_NAMESPACE_BEGIN

Capsule::Capsule(const Point3D& up_center, const Point3D& down_center, Float radius) : m_up_center(up_center), m_down_center(down_center), m_radius(radius) {}

Point3D Capsule::up_center() const
{
    return m_up_center;
}

Point3D Capsule::down_center() const
{
    return m_down_center;
}

Float Capsule::height() const
{
    return (m_up_center - m_down_center).norm() + 2 * m_radius;
}

Float Capsule::radius() const
{
    return m_radius;
}

Cuboid Capsule::aabb() const
{
    return Cuboid(0, 0, 0, 1, 1, 1);
}

DirectionDetection Capsule::detect_point_direction(const Point3D& point) const
{
    Vector3D h = point - m_down_center;
    Vector3D d = m_up_center - m_down_center;
    Float t = h.dot(d) / d.dot(d);

    if (t < -Max_Allowable_Error || t > 1.0 + Max_Allowable_Error)
    {
        Float d1 = h.norm();
        Vector3D hh = point - m_up_center;
        Float d2 = hh.norm();
        if (d1 < m_radius - Max_Allowable_Error || d2 < m_radius - Max_Allowable_Error)
            return DirectionDetection::e_inner;
        else if (EQUAL_TO_ZERO(d1 - m_radius) || EQUAL_TO_ZERO(d2 - m_radius))
            return DirectionDetection::e_border;
        else
            return DirectionDetection::e_outer;
    }
    else
    {
        Point3D q = m_down_center + t * d;
        Float distance = (q - point).norm();
        if (distance < m_radius - Max_Allowable_Error)
                return DirectionDetection::e_inner;
        else if (EQUAL_TO_ZERO(distance - m_radius))
            return DirectionDetection::e_border;
        else
            return DirectionDetection::e_outer;
    }
}

AMAZING_NAMESPACE_END