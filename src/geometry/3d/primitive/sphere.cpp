//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/3d/primitive/sphere.h"

AMAZING_NAMESPACE_BEGIN

Sphere::Sphere(const Point3D& center, Float radius) : m_center(center), m_radius(radius) {}

PrimitiveType Sphere::type() const
{
    return PrimitiveType::e_sphere;
}

Point3D Sphere::center() const
{
    return m_center;
}

Float Sphere::radius() const
{
    return m_radius;
}

AABB Sphere::aabb() const
{
    Vector3D half(m_radius, m_radius, m_radius);
    return {m_center - half, m_center + half};
}

DirectionDetection Sphere::detect_point_direction(const Point3D& point) const
{
    Vector3D v = point - m_center;
    Float d_squared = v.dot(v);
    Float r_squared = m_radius * m_radius;
    if (d_squared > r_squared + Max_Allowable_Error)
        return DirectionDetection::e_outer;
    else if (d_squared < r_squared - Max_Allowable_Error)
        return DirectionDetection::e_inner;
    else
        return DirectionDetection::e_border;
}


AMAZING_NAMESPACE_END