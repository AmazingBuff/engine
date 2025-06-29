//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef ENUMERATION_H
#define ENUMERATION_H

#include <astd/astd.h>

AMAZING_NAMESPACE_BEGIN

enum class GPUQueueType : uint8_t
{
    e_graphics,
    e_compute,
    e_transfer,

    e_count
};
static constexpr uint8_t GPU_Queue_Type_Count = to_underlying(GPUQueueType::e_count);

enum class GPUTextureType : uint8_t
{
    e_1d,
    e_1d_array,
    e_2d,
    e_2d_array,
    e_2dms,
    e_2dms_array,
    e_3d,
    e_cube,
    e_cube_array,

    e_count,
    e_undefined
};
static constexpr uint8_t GPU_Texture_Type_Count = to_underlying(GPUTextureType::e_count);

enum class GPUFormat : uint8_t
{
    e_undefined,

    e_r1_unorm,

    e_r4g4_unorm,

    e_r8_unorm,
    e_r8_snorm,
    e_r8_uint,
    e_r8_sint,
    e_r8_srgb,

    e_r8g8_unorm,
    e_r8g8_snorm,
    e_r8g8_uint,
    e_r8g8_sint,
    e_r8g8_srgb,

    e_r5g6b5_unorm,
    e_b5g6r5_unorm,
    e_r5g5b5a1_unorm,
    e_b5g5r5a1_unorm,
    e_r4g4b4a4_unorm,
    e_b4g4r4a4_unorm,

    e_r8g8b8_unorm,
    e_r8g8b8_snorm,
    e_r8g8b8_uint,
    e_r8g8b8_sint,
    e_r8g8b8_srgb,

    e_b8g8r8_unorm,
    e_b8g8r8_snorm,
    e_b8g8r8_uint,
    e_b8g8r8_sint,
    e_b8g8r8_srgb,

    e_r8g8b8a8_unorm,
    e_r8g8b8a8_snorm,
    e_r8g8b8a8_uint,
    e_r8g8b8a8_sint,
    e_r8g8b8a8_srgb,

    e_b8g8r8a8_unorm,
    e_b8g8r8a8_snorm,
    e_b8g8r8a8_uint,
    e_b8g8r8a8_sint,
    e_b8g8r8a8_srgb,

    e_b8g8r8x8_unorm,

    e_r10g10b10a2_unorm,
    e_r10g10b10a2_uint,

    e_r16_unorm,
    e_r16_snorm,
    e_r16_uint,
    e_r16_sint,
    e_r16_sfloat,

    e_r16g16_unorm,
    e_r16g16_snorm,
    e_r16g16_uint,
    e_r16g16_sint,
    e_r16g16_sfloat,

    e_r16g16b16_unorm,
    e_r16g16b16_snorm,
    e_r16g16b16_uint,
    e_r16g16b16_sint,
    e_r16g16b16_sfloat,

    e_r16g16b16a16_unorm,
    e_r16g16b16a16_snorm,
    e_r16g16b16a16_uint,
    e_r16g16b16a16_sint,
    e_r16g16b16a16_sfloat,

    e_r32_uint,
    e_r32_sint,
    e_r32_sfloat,

    e_r32g32_uint,
    e_r32g32_sint,
    e_r32g32_sfloat,

    e_r32g32b32_uint,
    e_r32g32b32_sint,
    e_r32g32b32_sfloat,

    e_r32g32b32a32_uint,
    e_r32g32b32a32_sint,
    e_r32g32b32a32_sfloat,

    e_r64_uint,
    e_r64_sint,
    e_r64_sfloat,

    e_r64g64_uint,
    e_r64g64_sint,
    e_r64g64_sfloat,

    e_r64g64b64_uint,
    e_r64g64b64_sint,
    e_r64g64b64_sfloat,

    e_r64g64b64a64_uint,
    e_r64g64b64a64_sint,
    e_r64g64b64a64_sfloat,

    e_d24_unorm_s8_uint,
    e_d32_sfloat_s8_uint,
    e_d24_sfloat,
    e_d32_sfloat,
    e_d24_x8_unorm,
    e_d16_unorm,
    e_d16_unorm_s8_uint,
    e_s8_uint,

    e_count
};
static constexpr uint8_t GPU_Format_Count = to_underlying(GPUFormat::e_count);


// mid
enum class GPUSampleCount : uint8_t
{
    e_1 = 1,
    e_2 = 2,
    e_4 = 4,
    e_8 = 8,
    e_16 = 16,
    e_32 = 32,
    e_64 = 64
};

enum class REFLECT GPUFilterType : uint8_t
{
    e_nearset,
    e_linear,
};

enum class REFLECT GPUMipMapMode : uint8_t
{
    e_nearset,
    e_linear,
};

enum class REFLECT GPUAddressMode : uint8_t
{
    e_repeat,
    e_mirror,
    e_clamp_to_edge,
    e_clamp_to_border,
};

enum class GPUMemoryPoolType : uint8_t
{
    e_automatic,
    e_linear,
    e_tiled
};

enum class GPUBlendConstant : uint8_t
{
    e_zero,
    e_one,
    e_src_color,
    e_one_minus_src_color,
    e_dst_color,
    e_one_minus_dst_color,
    e_src_alpha,
    e_one_minus_src_alpha,
    e_dst_alpha,
    e_one_minus_dst_alpha,
    e_src_alpha_saturate,
    e_blend_factor,
    e_one_minus_blend_factor,
};

enum class GPUBlendMode : uint8_t
{
    e_add,
    e_subtract,
    e_reverse_subtract,
    e_min,
    e_max,
};

enum class GPUStencilOp : uint8_t
{
    e_keep,
    e_zero,
    e_replace,
    e_invert,
    e_increment,
    e_decrement,
    e_increment_saturate,
    e_decrement_saturate,
};

enum class GPUFillMode : uint8_t
{
    e_solid,
    e_wire_frame,
};

enum class GPUCullMode : uint8_t
{
    e_none,
    e_back,
    e_front,
    e_both
};

enum class GPUFrontFace : uint8_t
{
    e_counter_clockwise,
    e_clockwise,
};

enum class GPUBackend : uint8_t
{
    e_vulkan,
    e_d3d12,
};

enum class GPUQueryType : uint8_t
{
    e_occlusion,
    e_timestamp,
    e_pipeline_statistics,
};

enum class GPUMemoryUsage : uint8_t
{
    e_unknown,
    e_gpu_only,
    e_cpu_only,
    e_cpu_to_gpu,
    e_gpu_to_cpu,
};

enum class GPUCompareMode : uint8_t
{
    e_never,
    e_less,
    e_equal,
    e_less_equal,
    e_greater,
    e_not_equal,
    e_greater_equal,
    e_always,
};

enum class GPUPipelineType : uint8_t
{
    e_graphics,
    e_compute,
    e_ray_tracing,
};

enum class GPUVertexInputRate : uint8_t
{
    e_vertex,
    e_instance,
};

enum class GPUPrimitiveTopology : uint8_t
{
    e_point_list,
    e_line_list,
    e_line_strip,
    e_triangle_list,
    e_triangle_strip,
    e_patch_list,
};

enum class GPULoadAction : uint8_t
{
    e_dont_care,
    e_load,
    e_clear
};

enum class GPUStoreAction : uint8_t
{
    e_dont_care,
    e_store,
    e_discard
};


// flags
enum class GPUShaderStage : uint8_t
{
    e_undefined = 0,
    e_vertex = 1,
    e_tessellation_control = 2,
    e_tessellation_evaluation = 4,
    e_geometry = 8,
    e_fragment = 16,
    e_compute = 32,
    e_ray_tracing = 64,

    e_all_graphics = e_vertex | e_tessellation_control | e_tessellation_evaluation | e_geometry | e_fragment,
    e_hull = e_tessellation_control,
    e_domain = e_tessellation_evaluation,
};
FLAG_ENUM(GPUShaderStage)

enum class GPUResourceType : uint32_t
{
    e_sampler = 1 << 0,
    e_texture = 1 << 1,
    e_render_target = 1 << 2,
    e_depth_stencil = 1 << 3,
    e_rw_texture = 1 << 4,
    e_buffer = 1 << 5,
    e_buffer_raw = (1 << 6) | e_buffer,
    e_rw_buffer = 1 << 7,
    e_rw_buffer_raw = (1 << 8) | e_rw_buffer,
    e_uniform_buffer = 1 << 9,
    e_push_constant = 1 << 10,
    e_vertex_buffer = 1 << 11,
    e_index_buffer = 1 << 12,
    e_indirect_buffer = 1 << 13,
    e_texture_cube = 1 << 14,
    e_render_target_mip_slices = 1 << 15,
    e_render_target_array_slices = 1 << 16,
    e_render_target_depth_slices = 1 << 17,
    e_ray_tracing = 1 << 18,

    // only valid in vulkan
    e_input_attachment = 1 << 24,
    e_texel_buffer = 1 << 25,
    e_rw_texel_buffer = 1 << 26,
    e_combined_image_sampler = 1 << 27, // sampler 2d
};
FLAG_ENUM(GPUResourceType)

enum class GPUResourceState : uint32_t
{
    e_undefined = 0,
    e_vertex_and_constant_buffer = 1 << 0,
    e_index_buffer = 1 << 1,
    e_render_target = 1 << 2,
    e_unordered_access = 1 << 3,
    e_depth_write = 1 << 4,
    e_depth_read = 1 << 5,
    e_non_pixel_shader_resource = 1 << 6,
    e_pixel_shader_resource = 1 << 7,
    e_shader_resource = e_non_pixel_shader_resource | e_pixel_shader_resource,
    e_stream_out = 1 << 8,
    e_indirect_argument = 1 << 9,
    e_copy_destination = 1 << 10,
    e_copy_source = 1 << 11,
    e_generic_read = e_vertex_and_constant_buffer | e_index_buffer | e_shader_resource | e_indirect_argument | e_copy_source,
    e_present = 1 << 12,
    e_common = 1 << 13,
    e_acceleration_structure = 1 << 14,
    e_shading_rate_source = 1 << 15,
    e_resolve_destination = 1 << 16,
};
FLAG_ENUM(GPUResourceState)

enum class GPUBufferFlag : uint8_t
{
    e_none = 0,
    e_dedicated = 1 << 0,
    e_persistent_map = 1 << 1,
    e_host_visible = 1 << 2,
};
FLAG_ENUM(GPUBufferFlag)

enum class GPUTextureFlag : uint32_t
{
    e_none = 0,
    e_dedicated = 1 << 0,
    e_export = 1 << 1,
    e_export_adapter = 1 << 2,
    e_on_tile = 1 << 3,
    e_no_compression = 1 << 4,
    e_force_2d = 1 << 5,
    e_force_3d = 1 << 6,
    e_allow_display_target = 1 << 7,
    e_normal_map = 1 << 8,
    e_fragment_mask = 1 << 9,
    e_aliasing_resource = 1 << 10,
    e_tiled_resource = 1 << 11,
};
FLAG_ENUM(GPUTextureFlag)

enum class GPUTextureViewUsage : uint8_t
{
    e_srv = 1,
    e_rtv_dsv = 2,
    e_uav = 4,
};
FLAG_ENUM(GPUTextureViewUsage)

enum class GPUTextureViewAspect : uint8_t
{
    e_color = 1,
    e_depth = 2,
    e_stencil = 4,
};
FLAG_ENUM(GPUTextureViewAspect)

enum class GPUDynamicState : uint64_t
{
    e_undefined = 0,
    e_cull_mode = 1ull << 0,
    e_front_face = 1ull << 1,
    e_primitive_topology = 1ull << 2,
    e_depth_test = 1ull << 3,
    e_depth_write = 1ull << 4,
    e_depth_compare = 1ull << 5,
    e_depth_bound_test = 1ull << 6,
    e_stencil_test = 1ull << 7,
    e_stencil_op = 1ull << 8,
    e_tier1 = (1ull << 9) - 1,

    e_raster_discard = 1ull << 9,
    e_depth_bias = 1ull << 10,
    e_primitive_restart = 1ull << 11,
    e_logic_op = 1ull << 12,
    e_patch_control_points = 1ull << 13,
    e_tier2 = (1ull << 14) - 1,

    e_tessellation_domain_origin = 1ull << 14,
    e_depth_clamp_enable = 1ull << 15,
    e_polygon_mode = 1ull << 16,
    e_sample_count = 1ull << 17,
    e_sample_mask = 1ull << 18,
    e_alpha_to_coverage_enable = 1ull << 19,
    e_alpha_to_one_enable = 1ull << 20,
    e_logic_op_enable = 1ull << 21,
    e_color_blend_enable = 1ull << 22,
    e_color_blend_equation = 1ull << 23,
    e_color_write_mask = 1ull << 24,
    e_raster_stream = 1ull << 25,
    e_conservative_raster_mode = 1ull << 26,
    e_extra_primitive_overestimation_size = 1ull << 27,
    e_depth_clip_enable = 1ull << 28,
    e_sample_locations_enable = 1ull << 29,
    e_color_blend_advanced = 1ull << 30,
    e_provoking_vertex_mode = 1ull << 31,
    e_line_rasterization_mode = 1ull << 32,
    e_line_stipple_enable = 1ull << 33,
    e_depth_clip_negative_one_to_one = 1ull << 34,
    e_viewport_scaling_enable = 1ull << 35,
    e_viewport_swizzle = 1ull << 36,
    e_coverage_to_color_enable = 1ull << 37,
    e_coverage_to_color_location = 1ull << 38,
    e_coverage_modulation_mode = 1ull << 39,
    e_coverage_modulation_table_enable = 1ull << 40,
    e_coverage_modulation_table = 1ull << 41,
    e_coverage_reduction_mode = 1ull << 42,
    e_representative_fragment_test_enable = 1ull << 43,
    e_shading_rate_image_enable = 1ull << 44,
    e_tier3 = (1ull << 45) - 1,
};
FLAG_ENUM(GPUDynamicState)







// util
static bool is_depth_stencil_format(GPUFormat const format)
{
    switch (format)
    {
    case GPUFormat::e_d24_unorm_s8_uint:
    case GPUFormat::e_d32_sfloat_s8_uint:
    case GPUFormat::e_d32_sfloat:
    case GPUFormat::e_d24_x8_unorm:
    case GPUFormat::e_d16_unorm:
    case GPUFormat::e_d16_unorm_s8_uint:
        return true;
    default:
        return false;
    }
}

static bool is_depth_only_format(GPUFormat const format)
{
    switch (format)
    {
    case GPUFormat::e_d16_unorm:
    case GPUFormat::e_d16_unorm_s8_uint:
        return true;
    default:
        return false;
    }
}


AMAZING_NAMESPACE_END

#endif //ENUMERATION_H
