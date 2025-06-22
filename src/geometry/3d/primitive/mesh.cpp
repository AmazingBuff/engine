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

AMAZING_NAMESPACE_END