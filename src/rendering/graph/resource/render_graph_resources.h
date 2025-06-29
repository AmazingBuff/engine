//
// Created by AmazingBuff on 2025/6/25.
//

#ifndef RENDER_GRAPH_RESOURCES_H
#define RENDER_GRAPH_RESOURCES_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

struct RenderGraphPipeline
{
    GPURootSignature* root_signature;
    GPUPipelineType pipeline_type;
    union
    {
        GPUGraphicsPipeline* graphics_pipeline;
        GPUComputePipeline* compute_pipeline;
    };
};


enum class RenderGraphResourceIOType : uint8_t
{
    e_read,
    e_write,
    e_read_write,
};

struct RenderGraphResource
{
    RenderGraphResourceType resource_type;
    union
    {
        struct
        {
            GPUTexture* texture;
            GPUTextureView* texture_view;
        } image;
        GPUBuffer* buffer;
    };
};

struct RenderGraphResourceBarrier
{
    GPUResourceState src_state;
    GPUResourceState dst_state;
};

AMAZING_NAMESPACE_END

#endif //RENDER_GRAPH_RESOURCES_H
