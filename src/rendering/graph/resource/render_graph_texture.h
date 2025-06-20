//
// Created by AmazingBuff on 2025/6/18.
//

#ifndef RENDER_GRAPH_TEXTURE_H
#define RENDER_GRAPH_TEXTURE_H

#include "rendering/rhi/create_info.h"
#include "rendering/render_type.h"

AMAZING_NAMESPACE_BEGIN

struct RenderGraphTexture
{
    GPUTexture* texture;
    GPUTextureView* texture_view;
};

RenderGraphTexture GPU_create_render_graph_texture(GPUDevice const* device, RenderGraphTextureCreateInfo const& info);
void GPU_destroy_render_graph_texture(RenderGraphTexture const& texture);

AMAZING_NAMESPACE_END

#endif //RENDER_GRAPH_TEXTURE_H
