//
// Created by AmazingBuff on 2025/6/5.
//

#include "geometry/3d/primitive/disc3d.h"

AMAZING_NAMESPACE_BEGIN

Disc3D::Disc3D(const Point3D& center, const Vector3D& normal, Float radius)
    : m_center(center), m_normal(normal.normalized()), m_radius(radius) {}

Point3D Disc3D::center() const
{
    return m_center;
}

Vector3D Disc3D::normal() const
{
    return m_normal;
}

Float Disc3D::radius() const
{
    return m_radius;
}

DirectionDetection Disc3D::detect_point_direction(const Point3D& point) const
{
    Vector3D p = point - m_center;
    Float t = p.dot(m_normal);
    // m_normal is normalized
    Vector3D p_prep = p - t * m_normal;
    Vector3D proj = p.dot(p_prep) / p_prep.dot(p_prep) * p_prep;
    if (proj.dot(proj) > m_radius * m_radius + Max_Allowable_Error)
        return DirectionDetection::e_outer;

    if (EQUAL_TO_ZERO(t))
    {
        // coplanar
        if (EQUAL_TO_ZERO(p.dot(p) - m_radius * m_radius))
            return DirectionDetection::e_border;
        else
            return DirectionDetection::e_inner;
    }
    else
        return DirectionDetection::e_outer;
}

AMAZING_NAMESPACE_END