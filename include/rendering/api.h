//
// Created by AmazingBuff on 2025/4/15.
//

#ifndef API_H
#define API_H

#include "core/math/algebra.h"

AMAZING_NAMESPACE_BEGIN

static constexpr uint32_t GPU_Node_Count = 1;
static constexpr uint32_t GPU_Node_Mask = 1;
static constexpr uint32_t GPU_Node_Index = 0;
static constexpr uint32_t GPU_Vendor_String_Length = 256;
static constexpr uint32_t GPU_Debug_Name_Length = 256;

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
};

struct RenderNode
{
    Affine3f transform;
    Vector<uint32_t> mesh_indices;
    Vector<RenderNode*> children;
};

AMAZING_NAMESPACE_END

#endif //API_H
