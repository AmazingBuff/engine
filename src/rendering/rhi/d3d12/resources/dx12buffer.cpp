//
// Created by AmazingBuff on 2025/4/18.
//

#include "dx12buffer.h"
#include "rendering/rhi/d3d12/dx12adapter.h"
#include "rendering/rhi/d3d12/dx12device.h"
#include "rendering/rhi/d3d12/internal/dx12descriptor_heap.h"
#include "rendering/rhi/d3d12/utils/dx_macro.h"
#include "rendering/rhi/d3d12/utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN

// for compatibility
FLAG_ENUM(D3D12MA::ALLOCATION_FLAGS);

DX12Buffer::DX12Buffer(GPUDevice const* device, GPUBufferCreateInfo const& info) : m_resource(nullptr), m_allocation(nullptr), m_gpu_address(0), m_handle{}, m_srv_offset(0), m_uav_offset(0)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);
    DX12Adapter const* dx12_adapter = static_cast<DX12Adapter const*>(dx12_device->m_ref_adapter);

    // allocate info
    size_t allocate_size = info.size;
    if (FLAG_IDENTITY(info.type, GPUResourceType::e_uniform_buffer))
    {
        size_t min_alignment = dx12_adapter->m_adapter_detail.uniform_buffer_alignment;
        allocate_size = align_to(allocate_size, min_alignment);
    }

    D3D12_RESOURCE_DESC buffer_desc{
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
        .Width = allocate_size,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = {
            .Count = 1,
            .Quality = 0,
        },
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE
    };

    if (FLAG_IDENTITY(info.type, GPUResourceType::e_rw_buffer))
        buffer_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    if (info.usage == GPUMemoryUsage::e_gpu_to_cpu)
        buffer_desc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

    // adjust
    size_t padded_size = 0;
    dx12_device->m_device->GetCopyableFootprints(&buffer_desc, 0, 1, 0, nullptr, nullptr, nullptr, &padded_size);
    if (padded_size != std::numeric_limits<size_t>::max())
    {
        allocate_size = padded_size;
        buffer_desc.Width = allocate_size;
    }

    GPUResourceState state = info.state;
    if (info.usage == GPUMemoryUsage::e_cpu_to_gpu || info.usage == GPUMemoryUsage::e_cpu_only)
        state = GPUResourceState::e_generic_read;
    else if (info.usage == GPUMemoryUsage::e_gpu_to_cpu)
        state = GPUResourceState::e_copy_destination;

    D3D12_RESOURCE_STATES buffer_state = transfer_resource_state(state);
    D3D12MA::ALLOCATION_DESC buffer_allocation_desc{
        .HeapType = transfer_heap_type(info.usage),
    };

    if (FLAG_IDENTITY(info.flags, GPUBufferFlag::e_dedicated))
        buffer_allocation_desc.Flags |= D3D12MA::ALLOCATION_FLAG_COMMITTED;

    if (buffer_allocation_desc.HeapType != D3D12_HEAP_TYPE_DEFAULT &&
        (buffer_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS))
    {
        D3D12_HEAP_PROPERTIES prop{
            .Type = D3D12_HEAP_TYPE_CUSTOM,
            .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE,
            .MemoryPoolPreference = D3D12_MEMORY_POOL_L0,
            .CreationNodeMask = GPU_Node_Mask,
            .VisibleNodeMask = GPU_Node_Mask,
        };
        if (buffer_state == D3D12_RESOURCE_STATE_GENERIC_READ)
            buffer_state = D3D12_RESOURCE_STATE_COMMON;
        DX_CHECK_RESULT(dx12_device->m_device->CreateCommittedResource(&prop, buffer_allocation_desc.ExtraHeapFlags, &buffer_desc, buffer_state, nullptr, IID_PPV_ARGS(&m_resource)));
    }
    else
    {
        DX_CHECK_RESULT(dx12_device->m_allocator->CreateResource(&buffer_allocation_desc, &buffer_desc, buffer_state, nullptr, &m_allocation, IID_PPV_ARGS(&m_resource)));
    }

    void* mapped_data = nullptr;
    if (FLAG_IDENTITY(info.flags, GPUBufferFlag::e_persistent_map))
    {
        if (FAILED(m_resource->Map(0, nullptr, &mapped_data)))
            RENDERING_LOG_WARNING("map buffer failed!");
    }

    m_gpu_address = m_resource->GetGPUVirtualAddress();

    // create descriptors
    DX12DescriptorHeap::D3D12DescriptorHeap* heap = dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
    uint32_t handle_count = (FLAG_IDENTITY(info.type, GPUResourceType::e_uniform_buffer) ? 1 : 0) +
        (FLAG_IDENTITY(info.type, GPUResourceType::e_buffer) ? 1 : 0) +
        (FLAG_IDENTITY(info.type, GPUResourceType::e_rw_buffer) ? 1 : 0);
    m_handle = DX12DescriptorHeap::consume_descriptor_handle(heap, handle_count).cpu;

    // cbv
    if (FLAG_IDENTITY(info.type, GPUResourceType::e_uniform_buffer))
    {
        D3D12_CPU_DESCRIPTOR_HANDLE cbv = m_handle;
        m_srv_offset = heap->descriptor_size;

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{
            .BufferLocation = m_gpu_address,
            .SizeInBytes = static_cast<uint32_t>(allocate_size),
        };
        dx12_device->m_descriptor_heap->create_cbv(dx12_device->m_device, cbv_desc, cbv);
    }

    // srv
    if (FLAG_IDENTITY(info.type, GPUResourceType::e_buffer))
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

        if (FLAG_IDENTITY(info.type, GPUResourceType::e_buffer_raw))
        {
            if (info.format != GPUFormat::e_undefined)
                RENDERING_LOG_WARNING("raw buffer use r32 typeless format. format will be ignored!");

            srv_desc.Format = DXGI_FORMAT_R32_TYPELESS;
            srv_desc.Buffer.Flags |= D3D12_BUFFER_SRV_FLAG_RAW;
        }

        if (srv_desc.Format != DXGI_FORMAT_UNKNOWN)
            srv_desc.Buffer.StructureByteStride = 0;

        dx12_device->m_descriptor_heap->create_srv(dx12_device->m_device, m_resource, srv_desc, srv);
    }

    // uav
    if (FLAG_IDENTITY(info.type, GPUResourceType::e_rw_buffer))
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

        if (FLAG_IDENTITY(info.type, GPUResourceType::e_rw_buffer_raw))
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
        dx12_device->m_descriptor_heap->create_uav(dx12_device->m_device, m_resource, counter_buffer_resource, uav_desc, uav);
    }

    // debug name
    if (!info.name.empty())
    {
        wchar_t debug_name[GPU_Debug_Name_Length];
        mbstowcs_s(nullptr, debug_name, info.name.c_str(), GPU_Debug_Name_Length);
        if (m_allocation)
            m_allocation->SetName(debug_name);
        DX_CHECK_RESULT(m_resource->SetName(debug_name));
    }

    m_info = Allocator<GPUBufferInfo>::allocate(1);
    m_info->size = allocate_size;
    m_info->type = info.type;
    m_info->mapped_data = mapped_data;
    m_info->memory_usage = info.usage;
    m_info->flags = info.flags;

    m_ref_device = dx12_device;
}

DX12Buffer::~DX12Buffer()
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(m_ref_device);
    if (m_handle.ptr != 0)
    {
        uint32_t handle_count = (FLAG_IDENTITY(m_info->type, GPUResourceType::e_uniform_buffer) ? 1 : 0) +
            (FLAG_IDENTITY(m_info->type, GPUResourceType::e_buffer) ? 1 : 0) +
            (FLAG_IDENTITY(m_info->type, GPUResourceType::e_rw_buffer) ? 1 : 0);
        DX12DescriptorHeap::return_descriptor_handle(dx12_device->m_descriptor_heap->m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV], m_handle, handle_count);
    }

    Allocator<GPUBufferInfo>::deallocate(m_info);
    DX_FREE(m_resource);
    DX_FREE(m_allocation);
}

void DX12Buffer::map(size_t offset, size_t size, const void* data)
{
    if (FLAG_IDENTITY(m_info->flags, GPUBufferFlag::e_persistent_map))
        memcpy(static_cast<uint8_t*>(m_info->mapped_data) + offset, data, size);
    else
    {
        D3D12_RANGE range{
            .Begin = offset,
            .End = offset + size,
        };

        DX_CHECK_RESULT(m_resource->Map(0, &range, &m_info->mapped_data));
    }
}

void DX12Buffer::unmap()
{
    m_resource->Unmap(0, nullptr);
    m_info->mapped_data = nullptr;
}



AMAZING_NAMESPACE_END
