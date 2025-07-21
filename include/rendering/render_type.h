//
// Created by AmazingBuff on 2025/6/17.
//

#ifndef RENDER_TYPE_H
#define RENDER_TYPE_H

#include "render_enum.h"

AMAZING_NAMESPACE_BEGIN

static constexpr uint32_t RENDER_Frame_Count = 3;

class RenderEntity;
class RenderView;
class RenderScene;
class RenderSystem;
class RenderGraph;
class RenderBuilder;

struct RenderSystemCreateInfo
{
    RenderBackend backend;

    void* window_handle;
    uint32_t window_width;
    uint32_t window_height;
    RenderFormat format;
};

struct RenderSceneCreateInfo
{
    RenderSystem const* render_system;
};


struct RenderShaderDescriptor
{
    uint32_t* code;
    uint32_t code_size;
    const char* entry;
    RenderShaderStage stage;
};

struct RenderRasterizerStateDescriptor
{
    struct
    {
        RenderCullMode cull_mode;
        RenderFillMode fill_mode;
        RenderFrontFace front_face;
    } rasterizer_state;

    RenderFormat const* color_format;
    RenderFormat depth_stencil_format;
    RenderPrimitiveTopology primitive_topology;
    uint8_t render_target_count;
};

struct RenderGraphCreateInfo
{
    RenderSystem const* render_system;
};

struct RenderGraphImageCreateInfo
{
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t array_layers;
    uint32_t mip_levels;
    RenderFormat format;
    RenderGraphImageUsage usage;
    RenderGraphImageType type;
    RenderGraphImageLayout layout;
};

struct RenderGraphBufferCreateInfo
{
    uint32_t size;
    RenderFormat format;
    RenderGraphBufferUsage usage;
    RenderGraphBufferType type;
    RenderGraphBufferLayout layout;
};

struct RenderGraphPipelineCreateInfo
{
    // universal info
    RenderShaderDescriptor const* shaders;
    const char* const* push_constant_names;
    const char* const* static_sampler_names;

    uint8_t shader_count;
    uint8_t push_constant_count;
    uint8_t static_sampler_count;

    // rasterizer info
    RenderRasterizerStateDescriptor const* rasterizer_descriptor;
};


// builtin parameter, all there parameters will be located in set/space 0
struct RenderObjectShaderDescriptor
{
    Affine3f model;
    Affine3f model_inv;
};

struct RenderPassShaderDescriptor
{
    Affine3f view;
    Affine3f projection;
};

struct RenderLightShaderDescriptor
{
    Vec3f position;
    Vec3f color;
};


using RenderGraphPassSetup = Functional<void(RenderBuilder&)>;
using RenderGraphPassExecute = Functional<void(RenderView&)>;

AMAZING_NAMESPACE_END

#endif //RENDER_TYPE_H
