//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/3d/primitive/triangle3d.h"
#include "geometry/3d/face3d.h"

AMAZING_NAMESPACE_BEGIN

Triangle3D::Triangle3D(const Point3D& p0, const Point3D& p1, const Point3D& p2)
    : m_vertices{p0, p1, p2} {}

Vector3D Triangle3D::normal() const
{
    Vector3D v1 = m_vertices[1] - m_vertices[0];
    Vector3D v2 = m_vertices[2] - m_vertices[0];

    return v1.cross(v2).normalized();
}

Float Triangle3D::d() const
{
    Vector3D o = m_vertices[0] - Point3D(0, 0, 0);
    return o.dot(normal());
}

Point3D Triangle3D::vertex(size_t i) const
{
    return m_vertices[i];
}

Float Triangle3D::area() const
{
    Vector3D v1 = m_vertices[1] - m_vertices[0];
    Vector3D v2 = m_vertices[2] - m_vertices[0];

    return v1.cross(v2).norm() / 2;
}

DirectionDetection Triangle3D::detect_point_direction(const Point3D& p) const
{
    Vector3D v1 = m_vertices[1] - m_vertices[0];
    Vector3D v2 = m_vertices[2] - m_vertices[0];

    Vector3D c = v1.cross(v2);

    Face3D face(m_vertices[0], c.normalized());
    if (face.detect_point_direction(p) == DirectionDetection::e_coplanar)
    {
        // o = a v1 + b v2
        Vector3D o = p - m_vertices[0];
        Vector3D u = o.cross(v2);
        Vector3D v = o.cross(v1);

        Float d = c.dot(c);
        Float a = u.dot(c) / d;
        Float b = -v.dot(c) / d;
        Float coef = a + b;
        if ((EQUAL_TO_ZERO(a) && b > Max_Allowable_Error && b < 1.0 - Max_Allowable_Error) ||
            (EQUAL_TO_ZERO(b) && a > Max_Allowable_Error && a < 1.0 - Max_Allowable_Error) ||
            EQUAL_TO_ZERO(coef) || EQUAL_TO_ZERO(1.0 - coef))
            return DirectionDetection::e_border;
        else if (a > Max_Allowable_Error && a < 1.0 - Max_Allowable_Error &&
            b > Max_Allowable_Error && b < 1.0 - Max_Allowable_Error &&
            coef > Max_Allowable_Error && coef < 1.0 - Max_Allowable_Error)
            return DirectionDetection::e_inner;
    }

    return DirectionDetection::e_outer;
}

AMAZING_NAMESPACE_END