//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_TYPE_H
#define RENDER_TYPE_H

#include "render_enum.h"

AMAZING_NAMESPACE_BEGIN

static constexpr uint32_t RENDER_Frame_Count = 3;


class RenderView;
class RenderScene;
class RenderSystem;
class RenderGraph;

struct RenderSystemCreateInfo
{
    RenderBackend backend;
};

struct RenderSceneCreateInfo
{
    RenderSystem const* render_system;
};

struct RenderViewCreateInfo
{
    RenderSystem const* render_system;
};


struct RenderGraphTextureCreateInfo
{
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t array_layers;
    uint32_t mip_levels;
    RenderFormat format;
};

AMAZING_NAMESPACE_END

#endif //RENDER_TYPE_H
