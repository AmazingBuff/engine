//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/3d/primitive/cone.h"

AMAZING_NAMESPACE_BEGIN

Cone::Cone(const Point3D& center, const Point3D& peak, Float radius) : m_center(center), m_peak(peak), m_radius(radius) {}

PrimitiveType Cone::type() const
{
    return PrimitiveType::e_cone;
}

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

AABB Cone::aabb() const
{
    // P = lerp(C2, C1, t) + t * R * (U * cos + V * sin)
    static auto compute_half = [](Float radius, Float n_component) -> Float
    {
        Float one_minus = 1.0 - n_component * n_component;
        if (EQUAL_TO_ZERO(one_minus))
            return 0;
        return radius * std::sqrt(one_minus);
    };

    Vector3D normal = (m_center - m_peak).normalized();

    Float half_x = compute_half(m_radius, normal.x());
    Float half_y = compute_half(m_radius, normal.y());
    Float half_z = compute_half(m_radius, normal.z());

    Point3D min(std::min(m_peak.x(), m_center.x() - half_x), std::min(m_peak.y(), m_center.y() - half_y), std::min(m_peak.z(), m_center.z() - half_z));
    Point3D max(std::max(m_peak.x(), m_center.x() + half_x), std::max(m_peak.y(), m_center.y() + half_y), std::max(m_peak.z(), m_center.z() + half_z));

    return {};
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