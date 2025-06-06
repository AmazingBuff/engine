//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/3d/primitive/cylinder.h"
#include "geometry/3d/primitive/cuboid.h"
#include "geometry/3d/segment3d.h"

AMAZING_NAMESPACE_BEGIN

Cylinder::Cylinder(const Point3D& up_center, const Point3D& down_center, Float radius)
    : m_up_center(up_center), m_down_center(down_center), m_radius(radius) {}

Point3D Cylinder::up_center() const
{
    return m_up_center;
}

Point3D Cylinder::down_center() const
{
    return m_down_center;
}

Float Cylinder::height() const
{
    return (m_up_center - m_down_center).norm();
}

Float Cylinder::radius() const
{
    return m_radius;
}

Cuboid Cylinder::aabb() const
{
    return Cuboid(0, 0, 0, 1, 1, 1);
}

DirectionDetection Cylinder::detect_point_direction(const Point3D& point) const
{
    Vector3D h = point - m_down_center;
    Vector3D d = m_up_center - m_down_center;
    Float t = h.dot(d) / d.dot(d);

    if (t < -Max_Allowable_Error || t > 1.0 + Max_Allowable_Error)
        return DirectionDetection::e_outer;
    else if (EQUAL_TO_ZERO(t) || EQUAL_TO_ZERO(t - 1.0))
        return DirectionDetection::e_border;
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