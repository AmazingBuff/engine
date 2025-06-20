//
// Created by AmazingBuff on 2025/6/5.
//

#include "geometry/3d/primitive/disc3d.h"

AMAZING_NAMESPACE_BEGIN

Disc3D::Disc3D(const Point3D& center, const Vector3D& normal, Float radius)
    : m_center(center), m_normal(normal.normalized()), m_radius(radius) {}

PrimitiveType Disc3D::type() const
{
    return PrimitiveType::e_disc;
}

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

AABB Disc3D::aabb() const
{
    // P = C + R * (U * cos + V * sin)
    static auto compute_half = [](Float radius, Float n_component) -> Float
    {
        Float one_minus = 1.0 - n_component * n_component;
        if (EQUAL_TO_ZERO(one_minus))
            return 0;
        return radius * std::sqrt(one_minus);
    };

    Float half_x = compute_half(m_radius, m_normal.x());
    Float half_y = compute_half(m_radius, m_normal.y());
    Float half_z = compute_half(m_radius, m_normal.z());

    Point3D min(m_center.x() - half_x, m_center.y() - half_y, m_center.z() - half_z);
    Point3D max(m_center.x() + half_x, m_center.y() + half_y, m_center.z() + half_z);

    return {min, max};
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