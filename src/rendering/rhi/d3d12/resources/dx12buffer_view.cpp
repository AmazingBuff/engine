//
// Created by AmazingBuff on 2025/7/7.
//

#include "dx12buffer_view.h"
#include "dx12buffer.h"
#include "rendering/rhi/d3d12/dx12device.h"
#include "rendering/rhi/d3d12/internal/dx12descriptor_heap.h"
#include "rendering/rhi/d3d12/utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN

DX12BufferView::DX12BufferView(GPUBufferViewCreateInfo const& info)
{
    DX12Buffer const* dx12_buffer = static_cast<DX12Buffer const*>(info.buffer);
    DX12Device const* dx12_device = static_cast<DX12Device const*>(dx12_buffer->m_ref_device);

    // create descriptors
    DX12DescriptorHeap::D3D12DescriptorHeap* heap = dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
    uint32_t handle_count = (FLAG_IDENTITY(info.usage, GPUBufferViewUsage::e_cbv) ? 1 : 0) +
        (FLAG_IDENTITY(info.usage, GPUBufferViewUsage::e_srv) ? 1 : 0) +
        (FLAG_IDENTITY(info.usage, GPUBufferViewUsage::e_uav) ? 1 : 0);
    m_handle = DX12DescriptorHeap::consume_descriptor_handle(heap, handle_count).cpu;

    // cbv
    if (FLAG_IDENTITY(info.usage, GPUBufferViewUsage::e_cbv))
    {
        D3D12_CPU_DESCRIPTOR_HANDLE cbv = m_handle;
        m_srv_offset = heap->descriptor_size;

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{
            .BufferLocation = dx12_buffer->m_resource->GetGPUVirtualAddress(),
            .SizeInBytes = static_cast<uint32_t>(dx12_buffer->m_info->size),
        };
        dx12_device->m_descriptor_heap->create_cbv(dx12_device->m_device, cbv_desc, cbv);
    }

    // srv
    if (FLAG_IDENTITY(info.usage, GPUBufferViewUsage::e_srv))
    {
        D3D12_CPU_DESCRIPTOR_HANDLE srv = { m_handle.ptr + m_srv_offset };
        m_uav_offset = m_srv_offset + heap->descriptor_size;

        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{
            .Format = transfer_format(info.format),
            .ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
            .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
            .Buffer{
                .FirstElement = info.data_array.first_element,
                .NumElements = info.data_array.element_count,
                .StructureByteStride = info.data_array.element_stride,
                .Flags = D3D12_BUFFER_SRV_FLAG_NONE
            }
        };

        if (FLAG_IDENTITY(dx12_buffer->m_info->type, GPUResourceType::e_buffer_raw))
        {
            if (info.format != GPUFormat::e_undefined)
                RENDERING_LOG_WARNING("raw buffer use r32 typeless format. format will be ignored!");

            srv_desc.Format = DXGI_FORMAT_R32_TYPELESS;
            srv_desc.Buffer.Flags |= D3D12_BUFFER_SRV_FLAG_RAW;
        }

        if (srv_desc.Format != DXGI_FORMAT_UNKNOWN)
            srv_desc.Buffer.StructureByteStride = 0;

        dx12_device->m_descriptor_heap->create_srv(dx12_device->m_device, dx12_buffer->m_resource, srv_desc, srv);
    }

    // uav
    if (FLAG_IDENTITY(info.usage, GPUBufferViewUsage::e_uav))
    {
        D3D12_CPU_DESCRIPTOR_HANDLE uav = { m_handle.ptr + m_uav_offset };

        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc{
            .Format = DXGI_FORMAT_UNKNOWN,
            .ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
            .Buffer{
                .FirstElement = info.data_array.first_element,
                .NumElements = info.data_array.element_count,
                .StructureByteStride = info.data_array.element_stride,
                .CounterOffsetInBytes = 0,
                .Flags = D3D12_BUFFER_UAV_FLAG_NONE
            },
        };

        if (FLAG_IDENTITY(dx12_buffer->m_info->type, GPUResourceType::e_rw_buffer_raw))
        {
            if (info.format != GPUFormat::e_undefined)
                RENDERING_LOG_WARNING("raw buffer use r32 typeless format. format will be ignored!");

            uav_desc.Format = DXGI_FORMAT_R32_TYPELESS;
            uav_desc.Buffer.Flags |= D3D12_BUFFER_UAV_FLAG_RAW;
        }
        else if (info.format != GPUFormat::e_undefined)
        {
            uav_desc.Format = transfer_format(info.format);
            D3D12_FEATURE_DATA_FORMAT_SUPPORT format_support{
                .Format = uav_desc.Format,
                .Support1 = D3D12_FORMAT_SUPPORT1_NONE,
                .Support2 = D3D12_FORMAT_SUPPORT2_NONE,
            };

            if (FAILED(dx12_device->m_device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &format_support, sizeof(format_support))) ||
                !(format_support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) ||
                !(format_support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE))
            {
                RENDERING_LOG_WARNING("uav format support failed!");
                uav_desc.Format = DXGI_FORMAT_UNKNOWN;
            }
        }

        if (uav_desc.Format != DXGI_FORMAT_UNKNOWN)
            uav_desc.Buffer.StructureByteStride = 0;

        ID3D12Resource* counter_buffer_resource = nullptr;
        if (info.counter_buffer)
            counter_buffer_resource = static_cast<DX12Buffer const*>(info.counter_buffer)->m_resource;
        dx12_device->m_descriptor_heap->create_uav(dx12_device->m_device, dx12_buffer->m_resource, counter_buffer_resource, uav_desc, uav);
    }

    m_ref_buffer = info.buffer;
    m_usage = info.usage;
    m_format = info.format;
}

DX12BufferView::~DX12BufferView()
{
    DX12Buffer const* dx12_buffer = static_cast<DX12Buffer const*>(m_ref_buffer);
    DX12Device const* dx12_device = static_cast<DX12Device const*>(dx12_buffer->m_ref_device);
    if (m_handle.ptr != 0)
    {
        uint32_t handle_count = (FLAG_IDENTITY(m_usage, GPUBufferViewUsage::e_cbv) ? 1 : 0) +
            (FLAG_IDENTITY(m_usage, GPUBufferViewUsage::e_srv) ? 1 : 0) +
            (FLAG_IDENTITY(m_usage, GPUBufferViewUsage::e_uav) ? 1 : 0);
        DX12DescriptorHeap::return_descriptor_handle(dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV], m_handle, handle_count);
    }
}

AMAZING_NAMESPACE_END