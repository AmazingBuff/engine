//
// Created by AmazingBuff on 2025/6/12.
//

#pragma once

#include "rendering/render_type.h"
#include "rendering/rhi/create_info.h"
#include "io/loader/scene_loader.h"

AMAZING_NAMESPACE_BEGIN

struct RenderVertex
{
    Vec3f position;
    Vec3f normal;
    Vec2f texcoord;
    Vec3f tangent;
    Vec3f bitangent;
};
static constexpr uint32_t RENDER_Vertex_Stride = sizeof(RenderVertex);

struct RenderMesh
{
    uint32_t vertex_offset;
    uint32_t index_offset;
    uint32_t vertex_count;
    uint32_t index_count;
    AABB aabb;
};

struct RenderTransform
{
    Affine3f model;
    uint32_t offset;
};

struct RenderNode
{
    RenderTransform transform;
    Vector<uint32_t> mesh_indices;
    Vector<RenderNode*> children;
};

struct RenderGeometry
{
    GPUBuffer const* vertex_buffer;
    GPUBuffer const* index_buffer;
    GPUBuffer const* uniform_buffer; // for transform

    RenderNode* root;
    Vector<RenderMesh> meshes;
};

RenderGeometry GPU_import_render_geometry(GPUDevice const* device, Scene const& scene);
RenderGeometry GPU_import_render_geometry(GPUDevice const* device, const char* file);
void GPU_destroy_render_geometry(RenderGeometry const& geometry);

AMAZING_NAMESPACE_END