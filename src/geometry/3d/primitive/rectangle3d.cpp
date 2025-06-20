//
// Created by AmazingBuff on 2025/6/4.
//

#include "geometry/3d/primitive/rectangle3d.h"
#include "geometry/3d/face3d.h"

AMAZING_NAMESPACE_BEGIN

Rectangle3D::Rectangle3D(const Point3D& o, const Point3D& h, const Point3D& v)
    : m_origin(o), m_horizontal(h), m_vertical(v){}

PrimitiveType Rectangle3D::type() const
{
    return PrimitiveType::e_rectangle;
}

Vector3D Rectangle3D::normal() const
{
    Vector3D v1 = m_horizontal - m_origin;
    Vector3D v2 = m_vertical - m_origin;

    return v1.cross(v2).normalized();
}

Float Rectangle3D::d() const
{
    Vector3D n = normal();
    Vector3D o = m_origin - Point3D(0, 0, 0);
    return o.dot(n);
}

Point3D Rectangle3D::origin() const
{
    return m_origin;
}

Vector3D Rectangle3D::horizontal() const
{
    return m_horizontal;
}

Vector3D Rectangle3D::vertical() const
{
    return m_vertical;
}

Float Rectangle3D::area() const
{
    Vector3D v1 = m_horizontal - m_origin;
    Vector3D v2 = m_vertical - m_origin;

    return v1.cross(v2).norm() / 2;
}

AABB Rectangle3D::aabb() const
{
    Point3D v1 = m_origin;
    Point3D v2 = m_horizontal;
    Point3D v3 = m_vertical;
    Point3D v4 = v2 + v3 - v1;

    Point3D min = std::min(std::min(std::min(v1, v2), v3), v4);
    Point3D max = std::max(std::max(std::max(v1, v2), v3), v4);

    return {min, max};
}

DirectionDetection Rectangle3D::detect_point_direction(const Point3D& p) const
{
    Vector3D v1 = m_horizontal - m_origin;
    Vector3D v2 = m_vertical - m_origin;

    Vector3D c = v1.cross(v2);

    Face3D face(m_origin, c.normalized());
    if (face.detect_point_direction(p) == DirectionDetection::e_coplanar)
    {
        // o = a v1 + b v2
        Vector3D o = p - m_origin;
        Vector3D u = o.cross(v2);
        Vector3D v = o.cross(v1);

        Float d = c.dot(c);
        Float a = u.dot(c) / d;
        Float b = -v.dot(c) / d;

        if (a > Max_Allowable_Error && a < 1.0 - Max_Allowable_Error &&
            b > Max_Allowable_Error && b < 1.0 - Max_Allowable_Error)
            return DirectionDetection::e_inner;
        else if (((EQUAL_TO_ZERO(a) || EQUAL_TO_ZERO(1.0 - a)) && b > Max_Allowable_Error && b < 1.0 - Max_Allowable_Error) ||
            ((EQUAL_TO_ZERO(b) || EQUAL_TO_ZERO(1.0 - b)) && a > Max_Allowable_Error && a < 1.0 - Max_Allowable_Error) ||
            ((EQUAL_TO_ZERO(a) || EQUAL_TO_ZERO(1.0 - a)) && (EQUAL_TO_ZERO(b) || EQUAL_TO_ZERO(1.0 - b))))
            return DirectionDetection::e_border;
    }

    return DirectionDetection::e_outer;
}

AMAZING_NAMESPACE_END