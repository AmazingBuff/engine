//
// Created by AmazingBuff on 2025/6/18.
//

#include "render_util.h"

AMAZING_NAMESPACE_BEGIN

GPUFormat transfer_format(RenderFormat format)
{
    switch (format)
    {
    case RenderFormat::e_b8g8r8a8: return GPUFormat::e_b8g8r8a8_snorm;
    case RenderFormat::e_r8g8b8a8: return GPUFormat::e_r8g8b8a8_snorm;
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

#undef TRANSFER_ENUM
AMAZING_NAMESPACE_END