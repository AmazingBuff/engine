//
// Created by AmazingBuff on 2025/4/17.
//

#include "dx12texture.h"
#include "rendering/rhi/d3d12/dx12device.h"
#include "rendering/rhi/d3d12/dx12adapter.h"
#include "rendering/rhi/d3d12/utils/dx_macro.h"
#include "rendering/rhi/d3d12/utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN

D3D12_RESOURCE_DIMENSION DX12Texture::transfer_resource_dimension(GPUTextureCreateInfo const& info)
{
    if (info.flags & GPUTextureFlagsFlag::e_force_2d)
        return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    else if (info.flags & GPUTextureFlagsFlag::e_force_3d)
        return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
    else
    {
        if (info.depth > 1)
            return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        else if (info.height > 1)
            return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        else
            return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
    }
}

D3D12_TEXTURE_LAYOUT DX12Texture::transfer_resource_layout(GPUTextureCreateInfo const& info)
{
    if (info.flags & GPUTextureFlagsFlag::e_tiled_resource)
        return D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE;
    else if (info.flags & GPUTextureFlagsFlag::e_export_adapter)
        return D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    else
        return D3D12_TEXTURE_LAYOUT_UNKNOWN;
}

D3D12_RESOURCE_FLAGS DX12Texture::transfer_resource_flags(GPUTextureCreateInfo const& info)
{
    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
    // uav
    if (info.type & GPUResourceTypeFlag::e_rw_texture)
        flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    // rtv
    if (info.type & GPUResourceTypeFlag::e_render_target)
        flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    else if (info.type & GPUResourceTypeFlag::e_depth_stencil)
        flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    // sharing
    if (info.flags & GPUTextureFlagsFlag::e_export_adapter)
        flags |= D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;

    return flags;
}

GPUResourceState DX12Texture::transfer_states(GPUTextureCreateInfo const& info)
{
    GPUResourceState state(GPUResourceStateFlag::e_undefined);
    if (info.state & GPUResourceStateFlag::e_copy_destination)
        state = GPUResourceStateFlag::e_common;
    else if (info.state & GPUResourceStateFlag::e_render_target)
    {
        if (info.state > GPUResourceStateFlag::e_render_target)
            state = info.state ^ GPUResourceStateFlag::e_render_target;
        else
            state = GPUResourceStateFlag::e_render_target;
    }
    else if (info.state & GPUResourceStateFlag::e_depth_write)
    {
        if (info.state > GPUResourceStateFlag::e_depth_write)
            state = info.state ^ GPUResourceStateFlag::e_depth_write;
        else
            state = GPUResourceStateFlag::e_depth_write;
    }

    if (info.flags & GPUTextureFlagsFlag::e_allow_display_target)
        state = GPUResourceStateFlag::e_present;

    return state;
}

uint32_t DX12Texture::transfer_sampler_count(ID3D12Device* device, DXGI_FORMAT format, uint32_t sample_count)
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaa{
        .Format = format,
        .SampleCount = sample_count,
        .Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
    };

    if (msaa.SampleCount > 1)
    {
        DX_CHECK_RESULT(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaa, sizeof(msaa)));
        while (msaa.NumQualityLevels == 0 && msaa.SampleCount > 1)
        {
            RENDERING_LOG_WARNING("sample count not supported, trying a lower sample count!");
            msaa.SampleCount = sample_count / 2;
            sample_count /= 2;
            DX_CHECK_RESULT(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaa, sizeof(msaa)));
        }

        return msaa.SampleCount;
    }
    return 1;
}

DX12Texture::DX12Texture() : m_resource(nullptr), m_allocation(nullptr) {}

DX12Texture::DX12Texture(GPUDevice const* device, GPUTextureCreateInfo const& info) : m_resource(nullptr), m_allocation(nullptr)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);
    DX12Adapter const* dx12_adapter = static_cast<DX12Adapter const*>(dx12_device->m_ref_adapter);

    DXGI_FORMAT dxgi_format = transfer_format(info.format);
    D3D12_RESOURCE_DESC desc{
        .Dimension = transfer_resource_dimension(info),
        .Alignment = to_underlying(info.sample_count) > 1 ? D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT : 0ull,
        .Width = info.width,
        .Height = info.height,
        .DepthOrArraySize = static_cast<uint16_t>(info.array_layers > 1 ? info.array_layers : info.depth),
        .MipLevels = static_cast<uint16_t>(info.mip_levels),
        .Format = format_typeless(dxgi_format),
        .SampleDesc{
            .Count = transfer_sampler_count(dx12_device->m_device, dxgi_format, to_underlying(info.sample_count)),
            .Quality = info.sample_quality,
        },
        .Layout = transfer_resource_layout(info),
        .Flags = transfer_resource_flags(info)
    };

    GPUResourceState state = info.state;
    state |= transfer_states(info);
    D3D12_RESOURCE_STATES resource_state = transfer_resource_state(state);

    D3D12_CLEAR_VALUE clear_value{
        .Format = dxgi_format,
    };

    if (info.type == GPUResourceTypeFlag::e_depth_stencil)
    {
        clear_value.DepthStencil.Depth = info.clear_color.depth_stencil.depth;
        clear_value.DepthStencil.Stencil = info.clear_color.depth_stencil.stencil;
    }
    else
    {
        clear_value.Color[0] = info.clear_color.color.r;
        clear_value.Color[1] = info.clear_color.color.g;
        clear_value.Color[2] = info.clear_color.color.b;
        clear_value.Color[3] = info.clear_color.color.a;
    }

    D3D12_CLEAR_VALUE* p_clear_value = nullptr;
    if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET || desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
        p_clear_value = &clear_value;

    if (info.flags & GPUTextureFlagsFlag::e_aliasing_resource)
    {
        D3D12AliasingTextureInfo* aliasing = Allocator<D3D12AliasingTextureInfo>::allocate(1);
        aliasing->resource_desc = desc;
        aliasing->name = info.name;
        m_info = aliasing;
        m_info->is_aliasing = 1;
    }
    else if (info.flags & GPUTextureFlagsFlag::e_tiled_resource)
    {
        DX_CHECK_RESULT(dx12_device->m_device->CreateReservedResource(&desc, resource_state, p_clear_value, IID_PPV_ARGS(&m_resource)));
        uint16_t layers = desc.DepthOrArraySize;

        // query
        uint32_t tile_count = 0;
        D3D12_PACKED_MIP_INFO packed_mip_info;
        D3D12_TILE_SHAPE tile_shape;
        uint32_t subresource_count = desc.MipLevels;
        D3D12_SUBRESOURCE_TILING* tiling = Allocator<D3D12_SUBRESOURCE_TILING>::allocate(subresource_count);
        dx12_device->m_device->GetResourceTiling(m_resource, &tile_count, &packed_mip_info, &tile_shape, &subresource_count, 0, tiling);

        D3D12TiledTextureInfo* tiled = Allocator<D3D12TiledTextureInfo>::allocate(1);
        tiled->tiled_info.tile_count = tile_count;
        tiled->tiled_info.tile_size = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

        tiled->tiled_info.tile_width_in_texels = tile_shape.WidthInTexels;
        tiled->tiled_info.tile_height_in_texels = tile_shape.HeightInTexels;
        tiled->tiled_info.tile_depth_in_texels = tile_shape.DepthInTexels;

        tiled->tiled_info.packed_mip_standard = packed_mip_info.NumStandardMips;
        tiled->tiled_info.packed_mip_count = packed_mip_info.NumPackedMips;

        tiled->subresources.reserve(subresource_count);
        for (uint32_t i = 0; i < subresource_count; i++)
        {
            D3D12TiledTextureInfo::D3D12TiledSubresourceMapping subresource{
                .allocation = nullptr,
                .subresource_info{
                    .layers = 0,
                    .mip_levels = i,
                    .width_in_tiles = tiling[i].WidthInTiles,
                    .height_in_tiles = tiling[i].HeightInTiles,
                    .depth_in_tiles = tiling[i].DepthInTiles,
                },
            };
            tiled->subresources[i] = subresource;
        }

        tiled->subresources.reserve(layers);
        for (uint16_t i = 0; i < layers; i++)
        {
            D3D12TiledTextureInfo::D3D12TiledPackingMipMapping packing{
                .allocation = nullptr,
                .count = packed_mip_info.NumTilesForPackedMips
            };
            tiled->packings[i] = packing;
        }

        m_info = tiled;
        m_info->is_tiled = 1;

        Allocator<D3D12_SUBRESOURCE_TILING>::deallocate(tiling);
    }
    else
    {
        m_info = Allocator<GPUTextureInfo>::allocate(1);

        D3D12MA::ALLOCATION_DESC alloc_desc{
            .HeapType = D3D12_HEAP_TYPE_DEFAULT,
        };

        if (info.flags & GPUTextureFlagsFlag::e_dedicated || info.sample_count > GPUSampleCount::e_1)
            alloc_desc.Flags |= D3D12MA::ALLOCATION_FLAG_COMMITTED;
        if (info.sample_count == GPUSampleCount::e_1 && !(info.flags & GPUTextureFlagsFlag::e_export))
            alloc_desc.Flags |= D3D12MA::ALLOCATION_FLAG_CAN_ALIAS;
        if (info.flags & GPUTextureFlagsFlag::e_export)
            alloc_desc.ExtraHeapFlags |= D3D12_HEAP_FLAG_SHARED;

        if (SUCCEEDED(dx12_device->m_allocator->CreateResource(&alloc_desc, &desc, resource_state, p_clear_value, &m_allocation, IID_PPV_ARGS(&m_resource))))
            RENDERING_LOG_INFO("create texture succeed!");
        else
        {
            D3D12_HEAP_PROPERTIES prop{
                .Type = D3D12_HEAP_TYPE_DEFAULT,
                .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
                .CreationNodeMask = GPU_Node_Mask,
                .VisibleNodeMask = GPU_Node_Mask,
            };

            if (SUCCEEDED(dx12_device->m_device->CreateCommittedResource(&prop, alloc_desc.ExtraHeapFlags, &desc, resource_state, p_clear_value, IID_PPV_ARGS(&m_resource))))
                RENDERING_LOG_INFO("create texture succeed with fallback driver API!");
            else
                RENDERING_LOG_ERROR("create texture failed with fallback driver API!");
        }
    }

    m_info->width = info.width;
    m_info->height = info.height;
    m_info->depth = info.depth;
    m_info->mip_levels = info.mip_levels;
    m_info->array_layers = info.array_layers;
    m_info->format = info.format;
    m_info->sample_count = info.sample_count;
    m_info->is_cube = (info.type & GPUResourceTypeFlag::e_texture_cube) == GPUResourceTypeFlag::e_texture_cube ? 1 : 0;

    if (m_resource)
    {
        const D3D12_RESOURCE_DESC resource_desc = m_resource->GetDesc();
        const D3D12_RESOURCE_ALLOCATION_INFO alloc_info = dx12_device->m_device->GetResourceAllocationInfo(GPU_Node_Mask, 1, &resource_desc);
        m_info->size_in_bytes = alloc_info.SizeInBytes;

        // debug name
        if (!info.name.empty())
        {
            wchar_t debug_name[GPU_Debug_Name_Length];
            mbstowcs_s(nullptr, debug_name, info.name.c_str(), GPU_Debug_Name_Length);
            if (m_allocation)
                m_allocation->SetName(debug_name);
            DX_CHECK_RESULT(m_resource->SetName(debug_name));
        }
    }
}

DX12Texture::~DX12Texture()
{
    if (m_info->is_aliasing)
        Allocator<D3D12AliasingTextureInfo>::deallocate(static_cast<D3D12AliasingTextureInfo*>(m_info));
    else if (m_info->is_tiled)
        Allocator<D3D12TiledTextureInfo>::deallocate(static_cast<D3D12TiledTextureInfo*>(m_info));
    else
        Allocator<GPUTextureInfo>::deallocate(m_info);

    DX_FREE(m_resource);
    DX_FREE(m_allocation);
}

AMAZING_NAMESPACE_END