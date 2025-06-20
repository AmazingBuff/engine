//
// Created by AmazingBuff on 2025/6/12.
//

#include "render_geometry.h"
#include "rendering/rhi/wrapper.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

AMAZING_NAMESPACE_BEGIN

static RenderMesh import_mesh(const aiMesh* mesh, const aiScene* scene, Vector<RenderVertex>& vertices, Vector<Index3i>& triangles, uint32_t& vertex_offset, uint32_t& triangle_offset)
{
    // position
    if (mesh->HasPositions())
    {
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            vertices[vertex_offset + i].position.x() = mesh->mVertices[i].x;
            vertices[vertex_offset + i].position.y() = mesh->mVertices[i].y;
            vertices[vertex_offset + i].position.z() = mesh->mVertices[i].z;
        }
    }

    // normal
    if (mesh->HasNormals())
    {
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            vertices[vertex_offset + i].normal.x() = mesh->mNormals[i].x;
            vertices[vertex_offset + i].normal.y() = mesh->mNormals[i].y;
            vertices[vertex_offset + i].normal.z() = mesh->mNormals[i].z;
        }
    }

    // texture coordinate
    if (mesh->HasTextureCoords(0))
    {
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            vertices[vertex_offset + i].texcoord.x() = mesh->mTextureCoords[0][i].x;
            vertices[vertex_offset + i].texcoord.y() = mesh->mTextureCoords[0][i].y;
        }
    }

    // tangent
    if (mesh->HasTangentsAndBitangents())
    {
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            vertices[vertex_offset + i].tangent.x() = mesh->mTangents[i].x;
            vertices[vertex_offset + i].tangent.y() = mesh->mTangents[i].y;
            vertices[vertex_offset + i].tangent.z() = mesh->mTangents[i].z;
            vertices[vertex_offset + i].bitangent.x() = mesh->mBitangents[i].x;
            vertices[vertex_offset + i].bitangent.y() = mesh->mBitangents[i].y;
            vertices[vertex_offset + i].bitangent.z() = mesh->mBitangents[i].z;
        }
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        Index3i index;
        for (uint32_t j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            index[j] = mesh->mFaces[i].mIndices[j];
        triangles[triangle_offset + i] = index;
    }

    vertex_offset += mesh->mNumVertices;
    triangle_offset += mesh->mNumFaces;

    AABB aabb{
        .min = {mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z},
        .max = {mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z}
    };
    return {vertex_offset, triangle_offset * 3, mesh->mNumVertices, mesh->mNumFaces * 3, aabb};
}

static RenderNode* import_node(const aiNode* node, const aiScene* scene, Vector<RenderMesh>& meshes, Vector<RenderVertex>& vertices, Vector<Index3i>& indices, Vector<Affine3f>& transforms, uint32_t mesh_offset, uint32_t& vertex_offset, uint32_t& triangle_offset, uint32_t& transform_offset)
{
    RenderNode *render_node = PLACEMENT_NEW(RenderNode, sizeof(RenderNode));
    memcpy(render_node->transform.model.data(), &node->mTransformation, std::min(sizeof(Affine3f), sizeof(node->mTransformation)));
    render_node->transform.offset = transform_offset * sizeof(Affine3f);
    transforms[transform_offset] = render_node->transform.model;
    transform_offset++;

    render_node->mesh_indices.resize(node->mNumMeshes);
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes[mesh_offset] = import_mesh(mesh, scene, vertices, indices, vertex_offset, triangle_offset);
        render_node->mesh_indices[i] = mesh_offset;
        mesh_offset++;
    }

    render_node->children.resize(node->mNumChildren);
    for (uint32_t i = 0; i < node->mNumChildren; i++)
      render_node->children[i] =
          import_node(node->mChildren[i], scene, meshes, vertices, indices, transforms,
                      mesh_offset, triangle_offset, vertex_offset, transform_offset);

    return render_node;
}

static RenderNode* transfer_render_node(Node* node, uint32_t& transform_offset, Vector<Affine3f>& transforms)
{
    RenderNode* render_node = PLACEMENT_NEW(RenderNode, sizeof(RenderNode));
    render_node->mesh_indices = node->mesh_indices;
    render_node->transform.model = node->transform;
    render_node->transform.offset = transform_offset;

    transform_offset += sizeof(Affine3f);
    transforms.push_back(render_node->transform.model);

    uint32_t child_size = node->children.size();
    render_node->children.resize(child_size);
    for (uint32_t i = 0; i < child_size; i++)
        render_node->children[i] = transfer_render_node(node->children[i], transform_offset, transforms);
    return render_node;
}

static void destroy_render_node(RenderNode* node)
{
    for (RenderNode* child : node->children)
        destroy_render_node(child);
    PLACEMENT_DELETE(RenderNode, node);
}


RenderGeometry GPU_import_render_geometry(GPUDevice const* device, const Scene& scene)
{
    uint32_t transform_offset = 0;
    Vector<Affine3f> transforms;
    RenderGeometry geometry;
    geometry.root = transfer_render_node(scene.root, transform_offset, transforms);

    uint32_t mesh_count = scene.meshes.size();
    geometry.meshes.resize(mesh_count);

    uint32_t vertex_count = 0;
    uint32_t triangle_count = 0;
    for (Mesh const* mesh : scene.meshes)
    {
        vertex_count += mesh->vertices.size();
        triangle_count += mesh->triangles.size();
    }
    Vector<RenderVertex> vertices(vertex_count);
    Vector<Index3i> triangles(triangle_count);

    uint32_t vertex_index = 0;
    uint32_t triangle_index = 0;
    for (uint32_t i = 0; i < mesh_count; i++)
    {
        Mesh const* mesh = scene.meshes[i];
        geometry.meshes[i].vertex_offset = vertex_index;
        geometry.meshes[i].index_offset = triangle_index * 3;
        geometry.meshes[i].vertex_count = mesh->vertices.size();
        geometry.meshes[i].index_count = mesh->triangles.size() * 3;
        geometry.meshes[i].aabb = mesh->aabb;

        for (uint32_t j = 0; j < mesh->triangles.size(); j++)
        {
            triangles[triangle_index].x() = mesh->triangles[j].x() + vertex_index;
            triangles[triangle_index].y() = mesh->triangles[j].y() + vertex_index;
            triangles[triangle_index].z() = mesh->triangles[j].z() + vertex_index;
            triangle_index++;
        }

        uint32_t local_vertex_index;

#define COPY_VERTEX_ATTRIBUTE(attribute, val)                   \
        local_vertex_index = vertex_index;                      \
        for (uint32_t j = 0; j < val.size(); j++)               \
        {                                                       \
            vertices[local_vertex_index].attribute = val[j];    \
            local_vertex_index++;                               \
        }

        COPY_VERTEX_ATTRIBUTE(position, mesh->vertices);
        COPY_VERTEX_ATTRIBUTE(normal, mesh->normals);
        COPY_VERTEX_ATTRIBUTE(texcoord, mesh->texcoords);
        COPY_VERTEX_ATTRIBUTE(tangent, mesh->tangents);
        COPY_VERTEX_ATTRIBUTE(bitangent, mesh->bitangents);
#undef COPY_VERTEX_ATTRIBUTE

        vertex_index = local_vertex_index;
    }

    GPUBufferCreateInfo info{
        .size = sizeof(vertices),
        .usage = GPUMemoryUsage::e_cpu_to_gpu,
        .type = GPUResourceTypeFlag::e_vertex_buffer,
        .flags = GPUBufferFlagsFlag::e_dedicated,
    };
    GPUBuffer* vertex_buffer = GPU_create_buffer(device, info);
    vertex_buffer->map(0, sizeof(vertices), vertices.data());

    info.size = sizeof(triangles);
    info.type = GPUResourceTypeFlag::e_index_buffer;
    GPUBuffer* index_buffer = GPU_create_buffer(device, info);
    index_buffer->map(0, sizeof(triangles), triangles.data());

    info.size = transform_offset;
    info.type = GPUResourceTypeFlag::e_uniform_buffer;
    info.flags = GPUBufferFlagsFlag::e_persistent_map;
    GPUBuffer* uniform_buffer = GPU_create_buffer(device, info);
    uniform_buffer->map(0, transform_offset, transforms.data());

    geometry.vertex_buffer = vertex_buffer;
    geometry.index_buffer = index_buffer;
    geometry.uniform_buffer = uniform_buffer;

    return geometry;
}

RenderGeometry GPU_import_render_geometry(GPUDevice const* device, const char* file)
{
    struct GeometryCounter
    {
        uint32_t vertex_count;
        uint32_t triangle_count;
        uint32_t mesh_count;
        uint32_t transform_count;
    };
    static auto count_geometry_attribute = [](this auto self, const aiNode* node, const aiScene* scene) -> GeometryCounter
    {
        uint32_t vertex_count = 0;
        uint32_t triangle_count = 0;
        uint32_t mesh_count = 0;
        uint32_t transform_count = 1;
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            vertex_count += mesh->mNumVertices;
            triangle_count += mesh->mNumFaces;
            mesh_count++;
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++)
        {
            GeometryCounter count = self(node->mChildren[i], scene);
            vertex_count += count.vertex_count;
            triangle_count += count.triangle_count;
            mesh_count += count.mesh_count;
            transform_count += count.transform_count;
        }
        return {vertex_count, triangle_count, mesh_count, transform_count};
    };

    uint32_t flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_MakeLeftHanded | aiProcess_GenBoundingBoxes;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        LOG_ERROR("Model Loader", "unable to load this model, which is from {}!", file);

    GeometryCounter count = count_geometry_attribute(scene->mRootNode, scene);
    Vector<RenderVertex> vertices(count.vertex_count);
    Vector<Index3i> triangles(count.triangle_count);
    Vector<Affine3f> transforms(count.transform_count);

    uint32_t vertex_offset = 0, triangle_offset = 0, mesh_offset = 0, transform_offset = 0;

    RenderGeometry geometry;
    geometry.meshes.reserve(count.mesh_count);
    geometry.root = import_node(scene->mRootNode, scene, geometry.meshes, vertices, triangles, transforms, mesh_offset, vertex_offset, triangle_offset, transform_offset);


    GPUBufferCreateInfo info{
        .size = sizeof(vertices),
        .usage = GPUMemoryUsage::e_cpu_to_gpu,
        .type = GPUResourceTypeFlag::e_vertex_buffer,
        .flags = GPUBufferFlagsFlag::e_dedicated,
    };
    GPUBuffer* vertex_buffer = GPU_create_buffer(device, info);
    vertex_buffer->map(0, sizeof(vertices), vertices.data());

    info.size = sizeof(triangles);
    info.type = GPUResourceTypeFlag::e_index_buffer;
    GPUBuffer* index_buffer = GPU_create_buffer(device, info);
    index_buffer->map(0, sizeof(triangles), triangles.data());

    info.size = transform_offset * sizeof(Affine3f);
    info.type = GPUResourceTypeFlag::e_uniform_buffer;
    info.flags = GPUBufferFlagsFlag::e_persistent_map;
    GPUBuffer* uniform_buffer = GPU_create_buffer(device, info);
    uniform_buffer->map(0, info.size, transforms.data());

    geometry.vertex_buffer = vertex_buffer;
    geometry.index_buffer = index_buffer;
    geometry.uniform_buffer = uniform_buffer;

    return geometry;
}

void GPU_destroy_render_geometry(const RenderGeometry& geometry)
{
    GPU_destroy_buffer(const_cast<GPUBuffer*>(geometry.vertex_buffer));
    GPU_destroy_buffer(const_cast<GPUBuffer*>(geometry.index_buffer));
    destroy_render_node(geometry.root);
}

AMAZING_NAMESPACE_END