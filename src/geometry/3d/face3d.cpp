//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/3d/face3d.h"

AMAZING_NAMESPACE_BEGIN

Face3D::Face3D(const Point3D& p, const Vector3D& normal) : m_normal(normal.normalized())
{
    Vector3D point = p - Point3D(0, 0, 0);
    m_d = p.dot(m_normal);
}

Face3D::Face3D(const Vector3D& normal, Float D) : m_normal(normal.normalized()), m_d(D) {}

Face3D::Face3D(Float A, Float B, Float C, Float D)
{
    Vector3D n = Vector3D(A, B, C);
    Float length = n.norm();

    m_normal = n / length;
    m_d = D / length;
}

Vector3D Face3D::normal() const
{
    return m_normal;
}

Float Face3D::d() const
{
    return m_d;
}

DirectionDetection Face3D::detect_point_direction(const Point3D& p) const
{
    Vector3D point;
    if (m_normal.x() > Max_Allowable_Error || m_normal.x() < -Max_Allowable_Error)
        point = {m_d / m_normal.x(), 0, 0};
    else if (m_normal.y() > Max_Allowable_Error || m_normal.y() < -Max_Allowable_Error)
        point = {0, m_d / m_normal.y(), 0};
    else
        point = {0, 0, m_d / m_normal.z()};

    Vector3D v = p - point;
    Float d = v.dot(m_normal);
    if (d > Max_Allowable_Error)
        return DirectionDetection::e_top;
    else if (d < -Max_Allowable_Error)
        return DirectionDetection::e_bottom;
    else
        return DirectionDetection::e_coplanar;
}




AMAZING_NAMESPACE_END