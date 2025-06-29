//
// Created by AmazingBuff on 2025/4/16.
//

#include "dx_utils.h"

AMAZING_NAMESPACE_BEGIN

DXGI_FORMAT transfer_format(GPUFormat format, bool shader)
{
    switch (format)
    {
    case GPUFormat::e_r1_unorm:
        return DXGI_FORMAT_R1_UNORM;
    case GPUFormat::e_r8_unorm:
        return DXGI_FORMAT_R8_UNORM;
    case GPUFormat::e_r8_snorm:
        return DXGI_FORMAT_R8_SNORM;
    case GPUFormat::e_r8_uint:
        return DXGI_FORMAT_R8_UINT;
    case GPUFormat::e_r8_sint:
        return DXGI_FORMAT_R8_SINT;
    case GPUFormat::e_r8g8_unorm:
        return DXGI_FORMAT_R8G8_UNORM;
    case GPUFormat::e_r8g8_snorm:
        return DXGI_FORMAT_R8G8_SNORM;
    case GPUFormat::e_r8g8_uint:
        return DXGI_FORMAT_R8G8_UINT;
    case GPUFormat::e_r8g8_sint:
        return DXGI_FORMAT_R8G8_SINT;
    case GPUFormat::e_r5g6b5_unorm:
        return DXGI_FORMAT_B5G6R5_UNORM;
    case GPUFormat::e_b5g6r5_unorm:
        return DXGI_FORMAT_B5G6R5_UNORM;
    case GPUFormat::e_b5g5r5a1_unorm:
        return DXGI_FORMAT_B5G5R5A1_UNORM;
    case GPUFormat::e_b4g4r4a4_unorm:
        return DXGI_FORMAT_B4G4R4A4_UNORM;
    case GPUFormat::e_r8g8b8a8_unorm:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case GPUFormat::e_r8g8b8a8_snorm:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
    case GPUFormat::e_r8g8b8a8_uint:
        return DXGI_FORMAT_R8G8B8A8_UINT;
    case GPUFormat::e_r8g8b8a8_sint:
        return DXGI_FORMAT_R8G8B8A8_SINT;
    case GPUFormat::e_r8g8b8a8_srgb:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    case GPUFormat::e_b8g8r8a8_unorm:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case GPUFormat::e_b8g8r8x8_unorm:
        return DXGI_FORMAT_B8G8R8X8_UNORM;
    case GPUFormat::e_b8g8r8a8_srgb:
        return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    case GPUFormat::e_r10g10b10a2_unorm:
        return DXGI_FORMAT_R10G10B10A2_UNORM;
    case GPUFormat::e_r10g10b10a2_uint:
        return DXGI_FORMAT_R10G10B10A2_UINT;
    case GPUFormat::e_r16_unorm:
        return DXGI_FORMAT_R16_UNORM;
    case GPUFormat::e_r16_snorm:
        return DXGI_FORMAT_R16_SNORM;
    case GPUFormat::e_r16_uint:
        return DXGI_FORMAT_R16_UINT;
    case GPUFormat::e_r16_sint:
        return DXGI_FORMAT_R16_SINT;
    case GPUFormat::e_r16_sfloat:
        return DXGI_FORMAT_R16_FLOAT;
    case GPUFormat::e_r16g16_unorm:
        return DXGI_FORMAT_R16G16_UNORM;
    case GPUFormat::e_r16g16_snorm:
        return DXGI_FORMAT_R16G16_SNORM;
    case GPUFormat::e_r16g16_uint:
        return DXGI_FORMAT_R16G16_UINT;
    case GPUFormat::e_r16g16_sint:
        return DXGI_FORMAT_R16G16_SINT;
    case GPUFormat::e_r16g16_sfloat:
        return DXGI_FORMAT_R16G16_FLOAT;
    case GPUFormat::e_r16g16b16a16_unorm:
        return DXGI_FORMAT_R16G16B16A16_UNORM;
    case GPUFormat::e_r16g16b16a16_snorm:
        return DXGI_FORMAT_R16G16B16A16_SNORM;
    case GPUFormat::e_r16g16b16a16_uint:
        return DXGI_FORMAT_R16G16B16A16_UINT;
    case GPUFormat::e_r16g16b16a16_sint:
        return DXGI_FORMAT_R16G16B16A16_SINT;
    case GPUFormat::e_r16g16b16a16_sfloat:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case GPUFormat::e_r32_uint:
        return DXGI_FORMAT_R32_UINT;
    case GPUFormat::e_r32_sint:
        return DXGI_FORMAT_R32_SINT;
    case GPUFormat::e_r32_sfloat:
        return DXGI_FORMAT_R32_FLOAT;
    case GPUFormat::e_r32g32_uint:
        return DXGI_FORMAT_R32G32_UINT;
    case GPUFormat::e_r32g32_sint:
        return DXGI_FORMAT_R32G32_SINT;
    case GPUFormat::e_r32g32_sfloat:
        return DXGI_FORMAT_R32G32_FLOAT;
    case GPUFormat::e_r32g32b32_uint:
        return DXGI_FORMAT_R32G32B32_UINT;
    case GPUFormat::e_r32g32b32_sint:
        return DXGI_FORMAT_R32G32B32_SINT;
    case GPUFormat::e_r32g32b32_sfloat:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case GPUFormat::e_r32g32b32a32_uint:
        return DXGI_FORMAT_R32G32B32A32_UINT;
    case GPUFormat::e_r32g32b32a32_sint:
        return DXGI_FORMAT_R32G32B32A32_SINT;
    case GPUFormat::e_r32g32b32a32_sfloat:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case GPUFormat::e_d16_unorm:
        return shader ? DXGI_FORMAT_R16_UNORM : DXGI_FORMAT_D16_UNORM;
    case GPUFormat::e_d32_sfloat:
        return shader ? DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_D32_FLOAT;
    case GPUFormat::e_d24_unorm_s8_uint:
    case GPUFormat::e_d24_x8_unorm:
        return shader ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_D24_UNORM_S8_UINT;
    case GPUFormat::e_d32_sfloat_s8_uint:
        return shader ? DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

D3D12_RESOURCE_STATES transfer_resource_state(GPUResourceState state)
{
    // these states cannot be combined with other states so we just do an == check
    if (state == GPUResourceState::e_generic_read)
        return D3D12_RESOURCE_STATE_GENERIC_READ;
    if (state == GPUResourceState::e_common)
        return D3D12_RESOURCE_STATE_COMMON;
    if (state == GPUResourceState::e_present)
        return D3D12_RESOURCE_STATE_PRESENT;

    D3D12_RESOURCE_STATES ret = D3D12_RESOURCE_STATE_COMMON;
    if (FLAG_IDENTITY(state, GPUResourceState::e_vertex_and_constant_buffer))
        ret |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    if (FLAG_IDENTITY(state, GPUResourceState::e_index_buffer))
        ret |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
    if (FLAG_IDENTITY(state, GPUResourceState::e_render_target))
        ret |= D3D12_RESOURCE_STATE_RENDER_TARGET;
    if (FLAG_IDENTITY(state, GPUResourceState::e_unordered_access))
        ret |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    if (FLAG_IDENTITY(state, GPUResourceState::e_depth_write))
        ret |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
    if (FLAG_IDENTITY(state, GPUResourceState::e_depth_read))
        ret |= D3D12_RESOURCE_STATE_DEPTH_READ;
    if (FLAG_IDENTITY(state, GPUResourceState::e_non_pixel_shader_resource))
        ret |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    if (FLAG_IDENTITY(state, GPUResourceState::e_pixel_shader_resource))
        ret |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    if (FLAG_IDENTITY(state, GPUResourceState::e_stream_out))
        ret |= D3D12_RESOURCE_STATE_STREAM_OUT;
    if (FLAG_IDENTITY(state, GPUResourceState::e_indirect_argument))
        ret |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
    if (FLAG_IDENTITY(state, GPUResourceState::e_copy_destination))
        ret |= D3D12_RESOURCE_STATE_COPY_DEST;
    if (FLAG_IDENTITY(state, GPUResourceState::e_copy_source))
        ret |= D3D12_RESOURCE_STATE_COPY_SOURCE;
    if (FLAG_IDENTITY(state, GPUResourceState::e_acceleration_structure))
        ret |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
    if (FLAG_IDENTITY(state, GPUResourceState::e_shading_rate_source))
        ret |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
    if (FLAG_IDENTITY(state, GPUResourceState::e_resolve_destination))
        ret |= D3D12_RESOURCE_STATE_RESOLVE_DEST;

    return ret;
}

D3D12_HEAP_TYPE transfer_heap_type(GPUMemoryUsage usage)
{
    switch (usage)
    {
    case GPUMemoryUsage::e_cpu_only:
    case GPUMemoryUsage::e_cpu_to_gpu:
        return D3D12_HEAP_TYPE_UPLOAD;
    case GPUMemoryUsage::e_gpu_to_cpu:
        return D3D12_HEAP_TYPE_READBACK;
    default:
        return D3D12_HEAP_TYPE_DEFAULT;
    }
}

D3D12_FILTER transfer_filter(GPUFilterType min, GPUFilterType mag, GPUMipMapMode mipmap, bool anisotropy, bool compare)
{
    if (anisotropy)
        return compare ? D3D12_FILTER_COMPARISON_ANISOTROPIC : D3D12_FILTER_ANISOTROPIC;

    int filter = to_underlying(min) << 4 | to_underlying(mag) << 2 | to_underlying(mipmap);
    int base = compare ? D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT : D3D12_FILTER_MIN_MAG_MIP_POINT;

    return static_cast<D3D12_FILTER>(base + filter);
}

D3D12_TEXTURE_ADDRESS_MODE transfer_address_mode(GPUAddressMode mode)
{
    switch (mode)
    {
    case GPUAddressMode::e_repeat:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    case GPUAddressMode::e_mirror:
        return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
    case GPUAddressMode::e_clamp_to_edge:
        return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    case GPUAddressMode::e_clamp_to_border:
        return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    default:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    }
}

D3D12_DESCRIPTOR_RANGE_TYPE transfer_resource_type(GPUResourceType type)
{
    switch (type)
    {
    case GPUResourceType::e_uniform_buffer:
    case GPUResourceType::e_push_constant:
        return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    case GPUResourceType::e_rw_buffer:
    case GPUResourceType::e_rw_texture:
        return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    case GPUResourceType::e_sampler:
        return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    case GPUResourceType::e_ray_tracing:
    case GPUResourceType::e_buffer:
    case GPUResourceType::e_texture:
        return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    default:
        RENDERING_LOG_ERROR("unsupported resource type!");
        return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    }
}

D3D12_SHADER_VISIBILITY transfer_shader_stage(GPUShaderStage stage)
{
    D3D12_SHADER_VISIBILITY ret = D3D12_SHADER_VISIBILITY_ALL;
    uint32_t stage_count = 0;
    if (stage == GPUShaderStage::e_compute || stage == GPUShaderStage::e_ray_tracing)
        return D3D12_SHADER_VISIBILITY_ALL;

#define SHADER_STAGE_JUDGE(gpu_phase, dx12_phase)  if (FLAG_IDENTITY(stage, gpu_phase)) { ret = dx12_phase; ++stage_count; }
    SHADER_STAGE_JUDGE(GPUShaderStage::e_vertex, D3D12_SHADER_VISIBILITY_VERTEX);
    SHADER_STAGE_JUDGE(GPUShaderStage::e_geometry, D3D12_SHADER_VISIBILITY_GEOMETRY);
    SHADER_STAGE_JUDGE(GPUShaderStage::e_hull, D3D12_SHADER_VISIBILITY_HULL);
    SHADER_STAGE_JUDGE(GPUShaderStage::e_domain, D3D12_SHADER_VISIBILITY_DOMAIN);
    SHADER_STAGE_JUDGE(GPUShaderStage::e_fragment, D3D12_SHADER_VISIBILITY_PIXEL);
#undef SHADER_STAGE_JUDGE

        return stage_count > 1 ? D3D12_SHADER_VISIBILITY_ALL : ret;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE transfer_primitive_topology(GPUPrimitiveTopology primitive)
{
    switch (primitive)
    {
    case GPUPrimitiveTopology::e_point_list:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case GPUPrimitiveTopology::e_line_list:
    case GPUPrimitiveTopology::e_line_strip:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case GPUPrimitiveTopology::e_triangle_list:
    case GPUPrimitiveTopology::e_triangle_strip:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case GPUPrimitiveTopology::e_patch_list:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    default:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    }
}

uint32_t transfer_subresource_index(uint32_t mip, uint32_t layer, uint32_t plane, uint32_t mip_levels, uint32_t array_layers)
{
    return mip + (layer + array_layers * plane) * mip_levels;
}

DXGI_FORMAT format_typeless(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return DXGI_FORMAT_R32G32B32A32_TYPELESS;
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return DXGI_FORMAT_R32G32B32_TYPELESS;

    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
        return DXGI_FORMAT_R16G16B16A16_TYPELESS;

    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
        return DXGI_FORMAT_R32G32_TYPELESS;

    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
        return DXGI_FORMAT_R10G10B10A2_TYPELESS;

    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
        return DXGI_FORMAT_R8G8B8A8_TYPELESS;
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
        return DXGI_FORMAT_R16G16_TYPELESS;

    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
        return DXGI_FORMAT_R32_TYPELESS;
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
        return DXGI_FORMAT_R8G8_TYPELESS;
    case DXGI_FORMAT_B4G4R4A4_UNORM: // just treats a 16 raw bits
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
        return DXGI_FORMAT_R16_TYPELESS;
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
        return DXGI_FORMAT_R8_TYPELESS;
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        return DXGI_FORMAT_BC1_TYPELESS;
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        return DXGI_FORMAT_BC2_TYPELESS;
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        return DXGI_FORMAT_BC3_TYPELESS;
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return DXGI_FORMAT_BC4_TYPELESS;
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
        return DXGI_FORMAT_BC5_TYPELESS;
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        return DXGI_FORMAT_R16_TYPELESS;

    case DXGI_FORMAT_R11G11B10_FLOAT:
        return DXGI_FORMAT_R11G11B10_FLOAT;

    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8X8_TYPELESS;

    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_TYPELESS;

    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
        return DXGI_FORMAT_BC6H_TYPELESS;

    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return DXGI_FORMAT_BC7_TYPELESS;

    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return DXGI_FORMAT_R32G8X24_TYPELESS;
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return DXGI_FORMAT_R24G8_TYPELESS;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return DXGI_FORMAT_R24G8_TYPELESS;

        // typeless just return the input format
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC7_TYPELESS:
        return format;

    case DXGI_FORMAT_R1_UNORM:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
    case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_YUY2:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
    case DXGI_FORMAT_NV11:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
    case DXGI_FORMAT_A8P8:
    case DXGI_FORMAT_P208:
    case DXGI_FORMAT_V208:
    case DXGI_FORMAT_V408:
    case DXGI_FORMAT_UNKNOWN:
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

uint32_t format_bit_size(GPUFormat format)
{
    switch (format)
    {
    case GPUFormat::e_r1_unorm:
        return 1;
    case GPUFormat::e_r8_unorm:
    case GPUFormat::e_r8_snorm:
    case GPUFormat::e_r8_uint:
    case GPUFormat::e_r8_sint:
        return 8;
    case GPUFormat::e_r8g8_unorm:
    case GPUFormat::e_r8g8_snorm:
    case GPUFormat::e_r8g8_uint:
    case GPUFormat::e_r8g8_sint:
    case GPUFormat::e_r5g6b5_unorm:
    case GPUFormat::e_b5g6r5_unorm:
    case GPUFormat::e_b5g5r5a1_unorm:
    case GPUFormat::e_b4g4r4a4_unorm:
    case GPUFormat::e_r16_unorm:
    case GPUFormat::e_r16_snorm:
    case GPUFormat::e_r16_uint:
    case GPUFormat::e_r16_sint:
    case GPUFormat::e_r16_sfloat:
    case GPUFormat::e_d16_unorm:
        return 16;
    case GPUFormat::e_r8g8b8a8_unorm:
    case GPUFormat::e_r8g8b8a8_snorm:
    case GPUFormat::e_r8g8b8a8_uint:
    case GPUFormat::e_r8g8b8a8_sint:
    case GPUFormat::e_r8g8b8a8_srgb:
    case GPUFormat::e_b8g8r8a8_unorm:
    case GPUFormat::e_b8g8r8x8_unorm:
    case GPUFormat::e_b8g8r8a8_srgb:
    case GPUFormat::e_r10g10b10a2_unorm:
    case GPUFormat::e_r10g10b10a2_uint:
    case GPUFormat::e_r16g16_unorm:
    case GPUFormat::e_r16g16_snorm:
    case GPUFormat::e_r16g16_uint:
    case GPUFormat::e_r16g16_sint:
    case GPUFormat::e_r16g16_sfloat:
    case GPUFormat::e_r32_uint:
    case GPUFormat::e_r32_sint:
    case GPUFormat::e_r32_sfloat:
    case GPUFormat::e_d32_sfloat:
    case GPUFormat::e_d24_unorm_s8_uint:
    case GPUFormat::e_d24_x8_unorm:
        return 32;
    case GPUFormat::e_r16g16b16a16_unorm:
    case GPUFormat::e_r16g16b16a16_snorm:
    case GPUFormat::e_r16g16b16a16_uint:
    case GPUFormat::e_r16g16b16a16_sint:
    case GPUFormat::e_r16g16b16a16_sfloat:
    case GPUFormat::e_r32g32_uint:
    case GPUFormat::e_r32g32_sint:
    case GPUFormat::e_r32g32_sfloat:
        return 64;
    case GPUFormat::e_r32g32b32_uint:
    case GPUFormat::e_r32g32b32_sint:
    case GPUFormat::e_r32g32b32_sfloat:
        return 96;
    case GPUFormat::e_r32g32b32a32_uint:
    case GPUFormat::e_r32g32b32a32_sint:
    case GPUFormat::e_r32g32b32a32_sfloat:
        return 128;
    case GPUFormat::e_d32_sfloat_s8_uint:
        return 40;
    }

    return 32;
}

AMAZING_NAMESPACE_END