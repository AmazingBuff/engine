//
// Created by AmazingBuff on 2025/6/18.
//

#include "render_util.h"
#include "rendering/render_entity.h"

AMAZING_NAMESPACE_BEGIN

GPUFormat transfer_format(RenderFormat format)
{
    switch (format)
    {
    case RenderFormat::e_b8g8r8a8: return GPUFormat::e_b8g8r8a8_unorm;
    case RenderFormat::e_r8g8b8a8: return GPUFormat::e_r8g8b8a8_unorm;
    case RenderFormat::e_d32: return GPUFormat::e_d32_sfloat;
    case RenderFormat::e_d24s8: return GPUFormat::e_d24_unorm_s8_uint;
    default: return GPUFormat::e_undefined;
    }
}

#define TRANSFER_ENUM(render_type, gpu_type, enum) case render_type::enum: return gpu_type::enum
GPUShaderStage transfer_shader_stage(RenderShaderStage stage)
{
    switch (stage)
    {
#define SHADER_STAGE_TRANSFER_ENUM(enum) TRANSFER_ENUM(RenderShaderStage, GPUShaderStage, enum)
        SHADER_STAGE_TRANSFER_ENUM(e_vertex);
        SHADER_STAGE_TRANSFER_ENUM(e_tessellation_control);
        SHADER_STAGE_TRANSFER_ENUM(e_tessellation_evaluation);
        SHADER_STAGE_TRANSFER_ENUM(e_geometry);
        SHADER_STAGE_TRANSFER_ENUM(e_fragment);
        SHADER_STAGE_TRANSFER_ENUM(e_compute);
        SHADER_STAGE_TRANSFER_ENUM(e_ray_tracing);
#undef SHADER_STAGE_TRANSFER_ENUM
    }
    return GPUShaderStage::e_all_graphics;
}

GPUCullMode transfer_cull_mode(RenderCullMode cull_mode)
{
    switch (cull_mode)
    {
    case RenderCullMode::e_none: return GPUCullMode::e_none;
    case RenderCullMode::e_front: return GPUCullMode::e_front;
    case RenderCullMode::e_back: return GPUCullMode::e_back;
    }
    return GPUCullMode::e_none;
}
GPUFillMode transfer_fill_mode(RenderFillMode fill_mode)
{
    switch (fill_mode)
    {
    case RenderFillMode::e_solid: return GPUFillMode::e_solid;
    case RenderFillMode::e_wire_frame: return GPUFillMode::e_wire_frame;
    }
    return GPUFillMode::e_solid;
}

GPUFrontFace transfer_front_face(RenderFrontFace front_face)
{
    switch (front_face)
    {
    case RenderFrontFace::e_clockwise: return GPUFrontFace::e_clockwise;
    case RenderFrontFace::e_counter_clockwise: return GPUFrontFace::e_counter_clockwise;
    }
    return GPUFrontFace::e_clockwise;
}

GPUPrimitiveTopology transfer_primitive_topology(RenderPrimitiveTopology topology)
{
    switch (topology)
    {
    case RenderPrimitiveTopology::e_triangle_list: return GPUPrimitiveTopology::e_triangle_list;
    case RenderPrimitiveTopology::e_triangle_strip: return GPUPrimitiveTopology::e_triangle_strip;
    }
    return GPUPrimitiveTopology::e_triangle_list;
}

GPUResourceState transfer_resource_state(RenderGraphImageLayout layout)
{
    GPUResourceState state = GPUResourceState::e_undefined;
    if (FLAG_IDENTITY(layout, RenderGraphImageLayout::e_rtv))
        state |= GPUResourceState::e_render_target;
    if (FLAG_IDENTITY(layout, RenderGraphImageLayout::e_dsv))
        state |= GPUResourceState::e_depth_write | GPUResourceState::e_depth_read;
    if (FLAG_IDENTITY(layout, RenderGraphImageLayout::e_srv))
        state |= GPUResourceState::e_pixel_shader_resource | GPUResourceState::e_non_pixel_shader_resource;
    if (FLAG_IDENTITY(layout, RenderGraphImageLayout::e_uav))
        state |= GPUResourceState::e_unordered_access;
    if (FLAG_IDENTITY(layout, RenderGraphImageLayout::e_copy_src))
        state |= GPUResourceState::e_copy_source;
    if (FLAG_IDENTITY(layout, RenderGraphImageLayout::e_copy_dst))
        state |= GPUResourceState::e_copy_destination;

    return state;
}

GPUResourceState transfer_resource_state(RenderGraphBufferLayout layout)
{
    GPUResourceState state = GPUResourceState::e_undefined;
    if (FLAG_IDENTITY(layout, RenderGraphBufferLayout::e_srv))
        state |= GPUResourceState::e_shader_resource;
    if (FLAG_IDENTITY(layout, RenderGraphBufferLayout::e_uav))
        state |= GPUResourceState::e_unordered_access;
    if (FLAG_IDENTITY(layout, RenderGraphBufferLayout::e_copy_src))
        state |= GPUResourceState::e_copy_source;
    if (FLAG_IDENTITY(layout, RenderGraphBufferLayout::e_copy_dst))
        state |= GPUResourceState::e_copy_destination;

    return state;
}

GPUResourceType transfer_resource_type(RenderGraphImageUsage usage)
{
    GPUResourceType type = GPUResourceType::e_undefined;
    if (FLAG_IDENTITY(usage, RenderGraphImageUsage::e_rtv_dsv))
        type |= GPUResourceType::e_render_target | GPUResourceType::e_depth_stencil;
    if (FLAG_IDENTITY(usage, RenderGraphImageUsage::e_srv))
        type |= GPUResourceType::e_texture;
    if (FLAG_IDENTITY(usage, RenderGraphImageUsage::e_uav))
        type |= GPUResourceType::e_rw_texture;

    return type;
}

GPUResourceType transfer_resource_type(RenderGraphBufferUsage usage)
{
    GPUResourceType type = GPUResourceType::e_undefined;
    if (FLAG_IDENTITY(usage, RenderGraphBufferUsage::e_cbv))
        type |= GPUResourceType::e_uniform_buffer;
    if (FLAG_IDENTITY(usage, RenderGraphBufferUsage::e_srv))
        type |= GPUResourceType::e_buffer;
    if (FLAG_IDENTITY(usage, RenderGraphBufferUsage::e_uav))
        type |= GPUResourceType::e_rw_buffer;

    return type;
}

GPUTextureType transfer_texture_type(RenderGraphImageType type)
{
    switch (type)
    {
    case RenderGraphImageType::e_2d:
        return GPUTextureType::e_2d;
    case RenderGraphImageType::e_3d:
        return GPUTextureType::e_3d;
    case RenderGraphImageType::e_cube:
        return GPUTextureType::e_cube;
    }
    return GPUTextureType::e_2d;
}

GPUTextureViewUsage transfer_texture_view_usage(RenderGraphImageUsage usage)
{
    GPUTextureViewUsage view = static_cast<GPUTextureViewUsage>(0);
    if (FLAG_IDENTITY(usage, RenderGraphImageUsage::e_rtv_dsv))
        view |= GPUTextureViewUsage::e_rtv_dsv;
    if (FLAG_IDENTITY(usage, RenderGraphImageUsage::e_srv))
        view |= GPUTextureViewUsage::e_srv;
    if (FLAG_IDENTITY(usage, RenderGraphImageUsage::e_uav))
        view |= GPUTextureViewUsage::e_uav;

    return view;
}

GPUBufferViewUsage transfer_texture_view_usage(RenderGraphBufferUsage usage)
{
    GPUBufferViewUsage view = static_cast<GPUBufferViewUsage>(0);
    if (FLAG_IDENTITY(usage, RenderGraphBufferUsage::e_cbv))
        view |= GPUBufferViewUsage::e_cbv;
    if (FLAG_IDENTITY(usage, RenderGraphBufferUsage::e_srv))
        view |= GPUBufferViewUsage::e_srv;
    if (FLAG_IDENTITY(usage, RenderGraphBufferUsage::e_uav))
        view |= GPUBufferViewUsage::e_uav;

    return view;
}

#undef TRANSFER_ENUM

RenderEntity generate_render_entity()
{
    static std::atomic<uint32_t> id = 1;
    RenderEntity ret(id.load(std::memory_order_acquire));
    ++id;
    return ret;
}

AMAZING_NAMESPACE_END