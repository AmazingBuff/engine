//
// Created by AmazingBuff on 2025/6/18.
//

#include "render_graph_texture.h"
#include "render_util.h"
#include "rendering/rhi/wrapper.h"

AMAZING_NAMESPACE_BEGIN

RenderGraphTexture GPU_create_render_graph_texture(GPUDevice const* device, RenderGraphTextureCreateInfo const& info)
{
    GPUTextureCreateInfo texture_info{};

    GPUTexture* texture = GPU_create_texture(device, texture_info);

    GPUTextureViewCreateInfo texture_view_info{

    };

    GPUTextureView* texture_view = GPU_create_texture_view(texture_view_info);

    RenderGraphTexture graph_texture{
        .texture = texture,
        .texture_view = texture_view,
    };
    return graph_texture;
}

void GPU_destroy_render_graph_texture(RenderGraphTexture const& texture)
{
    GPU_destroy_texture_view(texture.texture_view);
    GPU_destroy_texture(texture.texture);
}

AMAZING_NAMESPACE_END