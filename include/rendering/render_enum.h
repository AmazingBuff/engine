//
// Created by AmazingBuff on 2025/6/18.
//

#ifndef RENDER_ENUM_H
#define RENDER_ENUM_H

#include "core/math/algebra.h"

AMAZING_NAMESPACE_BEGIN

enum class RenderBackend : uint8_t
{
    e_d3d12 = 0,
    e_vulkan = 1,
};

enum class RenderFormat : uint8_t
{
    e_undefined,
    e_r8g8b8a8,
    e_b8g8r8a8,

    e_d32,
    e_d24s8
};

enum class RenderGraphImageUsage : uint8_t
{
    e_srv = 1,
    e_uav = 2,
    e_rtv = 4,
};
FLAG_ENUM(RenderGraphImageUsage)

enum class RenderGraphBufferUsage : uint8_t
{
    e_srv = 1,
    e_uav = 2,
    e_cbv = 4,
};
FLAG_ENUM(RenderGraphBufferUsage)

enum class RenderGraphResourceType : uint8_t
{
    e_buffer,
    e_image,
};

enum class RenderShaderStage : uint8_t
{
    e_vertex,
    e_tessellation_control,
    e_tessellation_evaluation,
    e_geometry,
    e_fragment,
    e_compute,
    e_ray_tracing,
};

enum class RenderCullMode : uint8_t
{
    e_none,
    e_front,
    e_back
};

enum class RenderFillMode : uint8_t
{
    e_solid,
    e_wire_frame
};

enum class RenderFrontFace : uint8_t
{
    e_counter_clockwise,
    e_clockwise,
};

enum class RenderPrimitiveTopology : uint8_t
{
    e_triangle_list,
    e_triangle_strip
};

AMAZING_NAMESPACE_END
#endif //RENDER_ENUM_H
