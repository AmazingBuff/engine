//
// Created by AmazingBuff on 2025/6/9.
//

#ifndef MESH_H
#define MESH_H

#include "primitive.h"

AMAZING_NAMESPACE_BEGIN

using Index2i = Eigen::Vector2<uint32_t>;
using Index3i = Eigen::Vector3<uint32_t>;

static constexpr int Mesh_Valid_Index = -1;

// fork from UE DynamicMesh
// todo: use unreal implementation
struct Edge
{
    Index2i vert;       // vertex index form the field vertices of Mesh
    Index2i tri;        // triangle index form the field triangles of Mesh
};

class Mesh final : public Primitive
{
public:
    DynamicVector<Vec3f> vertices;
    DynamicVector<Vec3f> normals;
    DynamicVector<Vec2f> texcoords;
    DynamicVector<Vec3f> tangents;
    DynamicVector<Vec3f> bitangents;
    DynamicVector<Index3i> triangles;

    DynamicVector<Edge> edges;
    HashMap<Vec3f, DynamicVector<Edge>> vertex_edges;
    HashMap<Index3i, DynamicVector<Edge>> triangle_edges;
public:
    explicit Mesh(const AABB& aabb);
    ~Mesh() override = default;
    NODISCARD PrimitiveType type() const override;
    NODISCARD AABB aabb() const override;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& p) const override;
private:
    AABB m_aabb;
};

AMAZING_NAMESPACE_END


#endif //MESH_H
