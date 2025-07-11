//
// Created by AmazingBuff on 25-6-22.
//

#include "geometry/3d/primitive/mesh.h"

AMAZING_NAMESPACE_BEGIN

Mesh::Mesh(const AABB& aabb) : m_aabb(aabb) {}

PrimitiveType Mesh::type() const
{
    return PrimitiveType::e_mesh;
}

AABB Mesh::aabb() const
{
    return m_aabb;
}

DirectionDetection Mesh::detect_point_direction(const Point3D& p) const
{

    return DirectionDetection{};
}

NODISCARD Mesh Mesh::subdivide(uint32_t degree) const
{
    for (auto& triangle : triangles)
    {
        Vec3f v1 = vertices[triangle.x()];
        Vec3f v2 = vertices[triangle.y()];
        Vec3f v3 = vertices[triangle.z()];



    }

    return Mesh(aabb());
}

AMAZING_NAMESPACE_END