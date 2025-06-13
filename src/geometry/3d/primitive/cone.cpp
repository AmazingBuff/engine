//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/3d/primitive/cone.h"
#include "geometry/3d/primitive/cuboid.h"

AMAZING_NAMESPACE_BEGIN

Cone::Cone(const Point3D& center, const Point3D& peak, Float radius) : m_center(center), m_peak(peak), m_radius(radius) {}

Point3D Cone::center() const
{
    return m_center;
}

Point3D Cone::peak() const
{
    return m_peak;
}

Float Cone::height() const
{
    return (m_peak - m_center).norm();
}

Float Cone::radius() const
{
    return m_radius;
}

Cuboid Cone::aabb() const
{

    return Cuboid(0, 0, 0, 1, 1, 1);
}

DirectionDetection Cone::detect_point_direction(const Point3D& point) const
{
    Vector3D h = point - m_center;
    Vector3D d = m_peak - m_center;
    Float t = h.dot(d) / d.dot(d);

    if (t < -Max_Allowable_Error || t > 1.0 + Max_Allowable_Error)
        return DirectionDetection::e_outer;
    else
    {
        Point3D q = m_center + t * d;
        Float distance = (q - point).norm();
        Float radius = (1.0 - t) * m_radius;
        if (distance < radius - Max_Allowable_Error)
        {
            if (EQUAL_TO_ZERO(t) || EQUAL_TO_ZERO(1.0 - t))
                return DirectionDetection::e_border;
            else
                return DirectionDetection::e_inner;
        }
        else if (EQUAL_TO_ZERO(distance - m_radius))
            return DirectionDetection::e_border;
        else
            return DirectionDetection::e_outer;
    }
}


AMAZING_NAMESPACE_END