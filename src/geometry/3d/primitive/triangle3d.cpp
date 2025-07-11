//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/3d/primitive/triangle3d.h"
#include "geometry/3d/primitive/mesh.h"
#include "geometry/3d/face3d.h"

AMAZING_NAMESPACE_BEGIN

Triangle3D::Triangle3D(const Point3D& p0, const Point3D& p1, const Point3D& p2)
    : m_vertices{p0, p1, p2} {}

PrimitiveType Triangle3D::type() const
{
    return PrimitiveType::e_triangle;
}

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

AABB Triangle3D::aabb() const
{
    Point3D min = std::min(std::min(m_vertices[0], m_vertices[1]), m_vertices[2]);
    Point3D max = std::max(std::max(m_vertices[0], m_vertices[1]), m_vertices[2]);

    return {min, max};
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

Mesh Triangle3D::as_mesh() const
{
    Mesh ret(aabb());

    ret.vertices.resize(3);
    ret.vertices[0] = m_vertices[0];
    ret.vertices[1] = m_vertices[1];
    ret.vertices[2] = m_vertices[2];

    Vec3f n = normal();
    ret.normals.resize(3);
    ret.normals[0] = n;
    ret.normals[1] = n;
    ret.normals[2] = n;

    ret.triangles.resize(1);
    ret.triangles[0] = {0, 1, 2};

    ret.edges.resize(3);
    ret.edges[0].tri.x() = 0;
    ret.edges[0].tri.y() = Mesh_Valid_Index;

    ret.edges[1].tri.x() = 0;
    ret.edges[1].tri.y() = Mesh_Valid_Index;

    ret.edges[2].tri.x() = 0;
    ret.edges[2].tri.y() = Mesh_Valid_Index;

    return ret;
}

AMAZING_NAMESPACE_END