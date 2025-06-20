//
// Created by AmazingBuff on 2025/6/9.
//

#ifndef MESH_H
#define MESH_H

#include "3d/primitive/primitive.h"

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

struct Mesh
{
    DynamicVector<Vec3f> vertices;
    DynamicVector<Vec3f> normals;
    DynamicVector<Vec2f> texcoords;
    DynamicVector<Vec3f> tangents;
    DynamicVector<Vec3f> bitangents;
    DynamicVector<Index3i> triangles;

    DynamicVector<Edge> edges;
    HashMap<Vec3f, DynamicVector<Edge>> vertex_edges;
    HashMap<Index3i, DynamicVector<Edge>> triangle_edges;

    AABB aabb;
};

struct Node
{
    String name;
    uint64_t name_hash;
    Affine3f transform;
    Vector<uint32_t> mesh_indices;
    Vector<Node*> children;
};

struct Scene
{
    String name;
    uint64_t name_hash;
    Node* root;
    Vector<Mesh*> meshes;
};


AMAZING_NAMESPACE_END


#endif //MESH_H
