//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef ENUMERATION_H
#define ENUMERATION_H

AMAZING_NAMESPACE_BEGIN

enum class GPUBackend : uint8_t
{
    e_vulkan,
    e_d3d12,

    e_count
};
static constexpr uint8_t GPU_Backend_Count = std::to_underlying(GPUBackend::e_count);

enum class GPUQueryType : uint8_t
{
    e_occlusion,
    e_timestamp,
    e_pipeline_statistics,

    e_count
};
static constexpr uint8_t GPU_Query_Type_Count = std::to_underlying(GPUQueryType::e_count);

enum class GPUQueueType : uint8_t
{
    e_graphics,
    e_compute,
    e_transfer,

    e_count
};
static constexpr uint8_t GPU_Queue_Type_Count = std::to_underlying(GPUQueueType::e_count);

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

    e_count
};
static constexpr uint8_t GPU_Texture_Type_Count = std::to_underlying(GPUTextureType::e_count);


enum class GPUFormat : uint8_t
{


    e_count
};
static constexpr uint8_t GPU_Format_Count = std::to_underlying(GPUFormat::e_count);


// flags
enum class GPUSampleCount : uint8_t
{
    e_1  = 1,
    e_2  = 2,
    e_4  = 4,
    e_8  = 8,
    e_16 = 16,
    e_32 = 32,
    e_64 = 64
};

enum GPUShaderStage : uint8_t
{
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



AMAZING_NAMESPACE_END

#endif //ENUMERATION_H
