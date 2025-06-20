//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/3d/primitive/cylinder.h"
#include "geometry/3d/segment3d.h"

AMAZING_NAMESPACE_BEGIN

Cylinder::Cylinder(const Point3D& up_center, const Point3D& down_center, Float radius)
    : m_up_center(up_center), m_down_center(down_center), m_radius(radius) {}

PrimitiveType Cylinder::type() const
{
    return PrimitiveType::e_cylinder;
}

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

AABB Cylinder::aabb() const
{
    // P = lerp(C2, C1, t) + R * (U * cos + V * sin)
    static auto compute_half = [](Float radius, Float n_component) -> Float
    {
        Float one_minus = 1.0 - n_component * n_component;
        if (EQUAL_TO_ZERO(one_minus))
            return 0;
        return radius * std::sqrt(one_minus);
    };

    Vector3D normal = (m_up_center - m_down_center).normalized();

    Float half_x = compute_half(m_radius, normal.x());
    Float half_y = compute_half(m_radius, normal.y());
    Float half_z = compute_half(m_radius, normal.z());

    Point3D min = std::min(m_up_center, m_down_center);
    Point3D max = std::max(m_up_center, m_down_center);

    min.x() -= half_x;
    min.y() -= half_y;
    min.z() -= half_z;

    max.x() += half_x;
    max.y() += half_y;
    max.z() += half_z;

    return {min, max};
}

DirectionDetection Cylinder::detect_point_direction(const Point3D& point) const
{
    Vector3D h = point - m_down_center;
    Vector3D d = m_up_center - m_down_center;
    Float t = h.dot(d) / d.dot(d);

    if (t < -Max_Allowable_Error || t > 1.0 + Max_Allowable_Error)
        return DirectionDetection::e_outer;
    else
    {
        Point3D q = m_down_center + t * d;
        Float distance = (q - point).norm();
        if (distance < m_radius - Max_Allowable_Error)
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