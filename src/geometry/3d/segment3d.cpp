//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/3d/segment3d.h"

AMAZING_NAMESPACE_BEGIN

Segment3D::Segment3D(const Point3D& p1, const Point3D& p2) : m_start(p1), m_end(p2) {}

Vector3D Segment3D::direction() const
{
    return m_end - m_start;
}

Point3D Segment3D::origin() const
{
    return m_start;
}

Float Segment3D::length() const
{
    return direction().norm();
}

DirectionDetection Segment3D::detect_point_direction(const Point3D& p) const
{
    Vector3D v = p - m_start;
    Vector3D d = m_end - m_start;
    Vector3D c = v.cross(d);
    if (EQUAL_TO_ZERO(c.dot(c)))
    {
        // v = t * d
        Float t;
        if (!EQUAL_TO_ZERO(d.x()))
            t = v.x() / d.x();
        else if (!EQUAL_TO_ZERO(d.y()))
            t = v.y() / d.y();
        else
            t = v.z() / d.z();

        if (t > Max_Allowable_Error && t < 1.0 - Max_Allowable_Error)
            return DirectionDetection::e_inner;
        else if (EQUAL_TO_ZERO(t) || EQUAL_TO_ZERO(1.0 - t))
            return DirectionDetection::e_border;
    }

    return DirectionDetection::e_outer;
}

Float Segment3D::distance(const Point3D& p) const
{
    Vector3D h = p - m_start;
    Vector3D d = m_end - m_start;
    Float t = h.dot(d) / d.dot(d);

    if (t < Max_Allowable_Error)
        t = 0;
    else if (t > 1.0 - Max_Allowable_Error)
        t = 1.0;

    Point3D q = m_start + t * d;

    return (p - q).norm();
}

AMAZING_NAMESPACE_END