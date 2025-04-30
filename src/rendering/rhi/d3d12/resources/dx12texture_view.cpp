//
// Created by AmazingBuff on 2025/4/22.
//

#include "dx12texture_view.h"
#include "dx12texture.h"
#include "rendering/rhi/d3d12/dx12device.h"
#include "rendering/rhi/d3d12/internal/dx12descriptor_heap.h"
#include "rendering/rhi/d3d12/utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN

DX12TextureView::DX12TextureView(GPUDevice const* device, GPUTextureViewCreateInfo const& info) : m_rtv_dsv_handle{}, m_srv_uva_handle{}, m_uav_offset(0)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);
    DX12Texture* dx12_texture = static_cast<DX12Texture*>(info.texture);

    const GPUTextureViewUsage usage = info.usage;
    uint32_t handle_count = ((usage & GPUTextureViewUsageFlag::e_srv) ? 1 : 0) + ((usage & GPUTextureViewUsageFlag::e_uav) ? 1 : 0);
    if (handle_count > 0)
    {
        DX12DescriptorHeap::D3D12DescriptorHeap* heap = dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
        m_rtv_dsv_handle = DX12DescriptorHeap::consume_descriptor_handle(heap, handle_count).cpu;

        // srv
        if (usage & GPUTextureViewUsageFlag::e_srv)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE srv = m_rtv_dsv_handle;
            D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{
                .Format = transfer_format(info.format, true),
                .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
            };

            switch (info.type)
            {
            case GPUTextureType::e_1d:
            {
                srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
                srv_desc.Texture1D.MostDetailedMip = info.base_mip_level;
                srv_desc.Texture1D.MipLevels = info.mip_levels;
            }
            break;
            case GPUTextureType::e_1d_array:
            {
                srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
                srv_desc.Texture1DArray.MostDetailedMip = info.base_mip_level;
                srv_desc.Texture1DArray.MipLevels = info.mip_levels;
                srv_desc.Texture1DArray.FirstArraySlice = info.base_array_layer;
                srv_desc.Texture1DArray.ArraySize = info.array_layers;
            }
            break;
            case GPUTextureType::e_2d:
            {
                srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                srv_desc.Texture2D.MostDetailedMip = info.base_mip_level;
                srv_desc.Texture2D.MipLevels = info.mip_levels;
                srv_desc.Texture2D.PlaneSlice = 0;
            }
            break;
            case GPUTextureType::e_2d_array:
            {
                srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                srv_desc.Texture2DArray.MostDetailedMip = info.base_mip_level;
                srv_desc.Texture2DArray.MipLevels = info.mip_levels;
                srv_desc.Texture2DArray.FirstArraySlice = info.base_array_layer;
                srv_desc.Texture2DArray.ArraySize = info.array_layers;
                srv_desc.Texture2DArray.PlaneSlice = 0;
            }
            break;
            case GPUTextureType::e_2dms:
            {
                srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
            }
            break;
            case GPUTextureType::e_2dms_array:
            {
                srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
                srv_desc.Texture2DMSArray.FirstArraySlice = info.base_array_layer;
                srv_desc.Texture2DMSArray.ArraySize = info.array_layers;
            }
            break;
            case GPUTextureType::e_3d:
            {
                srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
                srv_desc.Texture3D.MostDetailedMip = info.base_mip_level;
                srv_desc.Texture3D.MipLevels = info.mip_levels;
            }
            break;
            case GPUTextureType::e_cube:
            {
                srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                srv_desc.TextureCube.MostDetailedMip = info.base_mip_level;
                srv_desc.TextureCube.MipLevels = info.mip_levels;
            }
            break;
            case GPUTextureType::e_cube_array:
            {
                srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
                srv_desc.TextureCubeArray.MostDetailedMip = info.base_mip_level;
                srv_desc.TextureCubeArray.MipLevels = info.mip_levels;
                srv_desc.TextureCubeArray.First2DArrayFace = info.base_array_layer;
                srv_desc.TextureCubeArray.NumCubes = info.array_layers;
            }
            break;
            default:
                RENDERING_LOG_ERROR("unsupported texture type!");
                break;
            }
            dx12_device->m_descriptor_heap->create_srv(dx12_device->m_device, dx12_texture->m_resource, srv_desc, srv);

            m_uav_offset += heap->descriptor_size;
        }

        // uav
        if (usage & GPUTextureViewUsageFlag::e_uav)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE      uav{ m_rtv_dsv_handle.ptr + m_uav_offset };
            D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc{
                .Format = transfer_format(info.format, true)
            };

            switch (info.type)
            {
            case GPUTextureType::e_1d:
            {
                uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
                uav_desc.Texture1D.MipSlice = info.base_mip_level;
            }
            break;
            case GPUTextureType::e_1d_array:
            {
                uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
                uav_desc.Texture1DArray.MipSlice = info.base_mip_level;
                uav_desc.Texture1DArray.FirstArraySlice = info.base_array_layer;
                uav_desc.Texture1DArray.ArraySize = info.array_layers;
            }
            break;
            case GPUTextureType::e_2d:
            {
                uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
                uav_desc.Texture2D.MipSlice = info.base_mip_level;
                uav_desc.Texture2D.PlaneSlice = 0;
            }
            break;
            case GPUTextureType::e_2d_array:
            {
                uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
                uav_desc.Texture2DArray.MipSlice = info.base_mip_level;
                uav_desc.Texture2DArray.FirstArraySlice = info.base_array_layer;
                uav_desc.Texture2DArray.ArraySize = info.array_layers;
                uav_desc.Texture2DArray.PlaneSlice = 0;
            }
            break;
            case GPUTextureType::e_3d:
            {
                uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
                uav_desc.Texture3D.MipSlice = info.base_mip_level;
                uav_desc.Texture3D.FirstWSlice = info.base_array_layer;
                uav_desc.Texture3D.WSize = info.array_layers;
            }
            break;
            default:
                RENDERING_LOG_ERROR("unsupported texture type!");
                break;
            }
            dx12_device->m_descriptor_heap->create_uav(dx12_device->m_device, dx12_texture->m_resource, nullptr, uav_desc, uav);
        }
    }

    // rtv dsv
    if (usage & GPUTextureViewUsageFlag::e_rtv_dsv)
    {
        const bool is_dsv = is_depth_stencil_format(info.format);
        if (is_dsv)
        {
            DX12DescriptorHeap::D3D12DescriptorHeap* heap = dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV];
            m_rtv_dsv_handle = DX12DescriptorHeap::consume_descriptor_handle(heap, 1).cpu;
            D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{
                .Format = transfer_format(info.format)
            };
            switch (info.type)
            {
            case GPUTextureType::e_1d:
            {
                dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
                dsv_desc.Texture1D.MipSlice = info.base_mip_level;
            }
            break;
            case GPUTextureType::e_1d_array:
            {
                dsv_desc.ViewDimension                  = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
                dsv_desc.Texture1DArray.MipSlice        = info.base_mip_level;
                dsv_desc.Texture1DArray.FirstArraySlice = info.base_array_layer;
                dsv_desc.Texture1DArray.ArraySize       = info.array_layers;
            }
            break;
            case GPUTextureType::e_2dms:
            {
                dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
            }
            break;
            case GPUTextureType::e_2d:
            {
                dsv_desc.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE2D;
                dsv_desc.Texture2D.MipSlice = info.base_mip_level;
            }
            break;
            case GPUTextureType::e_2dms_array:
            {
                dsv_desc.ViewDimension                    = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
                dsv_desc.Texture2DMSArray.FirstArraySlice = info.base_array_layer;
                dsv_desc.Texture2DMSArray.ArraySize       = info.array_layers;
            }
            break;
            case GPUTextureType::e_2d_array:
            {
                dsv_desc.ViewDimension                  = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
                dsv_desc.Texture2DArray.MipSlice        = info.base_mip_level;
                dsv_desc.Texture2DArray.FirstArraySlice = info.base_array_layer;
                dsv_desc.Texture2DArray.ArraySize       = info.array_layers;
            }
            break;
            default:
                RENDERING_LOG_ERROR("unsupported texture type!");
                break;
            }
            dx12_device->m_descriptor_heap->create_dsv(dx12_device->m_device, dx12_texture->m_resource, dsv_desc, m_rtv_dsv_handle);
        }
        else
        {
            DX12DescriptorHeap::D3D12DescriptorHeap* heap = dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
            m_rtv_dsv_handle = DX12DescriptorHeap::consume_descriptor_handle(heap, 1).cpu;
            D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{
                .Format = transfer_format(info.format)
            };
            switch (info.type)
            {
            case GPUTextureType::e_1d:
            {
                rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
                rtv_desc.Texture1D.MipSlice = info.base_mip_level;
            }
            break;
            case GPUTextureType::e_1d_array:
            {
                rtv_desc.ViewDimension                  = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
                rtv_desc.Texture1DArray.MipSlice        = info.base_mip_level;
                rtv_desc.Texture1DArray.FirstArraySlice = info.base_array_layer;
                rtv_desc.Texture1DArray.ArraySize       = info.array_layers;
            }
            break;
            case GPUTextureType::e_2dms:
            {
                rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
            }
            break;
            case GPUTextureType::e_2d:
            {
                rtv_desc.ViewDimension      = D3D12_RTV_DIMENSION_TEXTURE2D;
                rtv_desc.Texture2D.MipSlice = info.base_mip_level;
            }
            break;
            case GPUTextureType::e_2dms_array:
            {
                rtv_desc.ViewDimension                    = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
                rtv_desc.Texture2DMSArray.FirstArraySlice = info.base_array_layer;
                rtv_desc.Texture2DMSArray.ArraySize       = info.array_layers;
            }
            break;
            case GPUTextureType::e_2d_array:
            {
                rtv_desc.ViewDimension                  = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                rtv_desc.Texture2DArray.MipSlice        = info.base_mip_level;
                rtv_desc.Texture2DArray.FirstArraySlice = info.base_array_layer;
                rtv_desc.Texture2DArray.ArraySize       = info.array_layers;
            }
            break;
            case GPUTextureType::e_3d:
            {
                rtv_desc.ViewDimension         = D3D12_RTV_DIMENSION_TEXTURE3D;
                rtv_desc.Texture3D.MipSlice    = info.base_mip_level;
                rtv_desc.Texture3D.FirstWSlice = info.base_array_layer;
                rtv_desc.Texture3D.WSize       = info.array_layers;
            }
            break;
            default:
                RENDERING_LOG_ERROR("unsupported texture type!");
                break;
            }
            dx12_device->m_descriptor_heap->create_rtv(dx12_device->m_device, dx12_texture->m_resource, rtv_desc, m_rtv_dsv_handle);
        }
    }

    m_ref_device = device;
    m_usage = info.usage;
    m_format = info.format;
}

DX12TextureView::~DX12TextureView()
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(m_ref_device);

    if (m_rtv_dsv_handle.ptr != 0 && m_usage & GPUTextureViewUsageFlag::e_rtv_dsv)
    {
        DX12DescriptorHeap::D3D12DescriptorHeap* heap;
        if (is_depth_stencil_format(m_format))
            heap = dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV];
        else
            heap = dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
        DX12DescriptorHeap::return_descriptor_handle(heap, m_rtv_dsv_handle, 1);
    }

    if (m_srv_uva_handle.ptr != 0)
    {
        uint32_t handle_count = ((m_usage & GPUTextureViewUsageFlag::e_srv) ? 1 : 0) + ((m_usage & GPUTextureViewUsageFlag::e_uav) ? 1 : 0);
        DX12DescriptorHeap::D3D12DescriptorHeap* heap = dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
        DX12DescriptorHeap::return_descriptor_handle(heap, m_srv_uva_handle, handle_count);
    }
}

AMAZING_NAMESPACE_END