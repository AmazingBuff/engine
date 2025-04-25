//
// Created by AmazingBuff on 2025/4/24.
//

#include "dx12descriptor_set.h"
#include "dx12device.h"
#include "dx12root_signature.h"
#include "resources/dx12sampler.h"
#include "resources/dx12texture_view.h"
#include "resources/dx12buffer.h"
#include "internal/dx12descriptor_heap.h"
#include "rendering/api.h"

AMAZING_NAMESPACE_BEGIN

static uint32_t descriptor_count_need(GPUShaderResource const& resource)
{
    if (resource.texture_type == GPUTextureType::e_1d_array ||
        resource.texture_type == GPUTextureType::e_2d_array ||
        resource.texture_type == GPUTextureType::e_2dms_array ||
        resource.texture_type == GPUTextureType::e_cube_array)
        return resource.size;

    return 1;
}


DX12DescriptorSet::DX12DescriptorSet() : m_cbv_srv_uav_handle(0), m_cbv_srv_uav_stride(0), m_sampler_handle(0), m_sampler_stride(0) {}

DX12DescriptorSet::~DX12DescriptorSet()
{

}

AResult DX12DescriptorSet::initialize(GPUDevice const* device, GPUDescriptorSetCreateInfo const& info)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);
    DX12RootSignature const* dx12_root_signature = static_cast<DX12RootSignature const*>(info.root_signature);

    DX12DescriptorHeap::D3D12DescriptorHeap* cbv_srv_uav_heap = dx12_device->m_descriptor_heap->m_gpu_cbv_srv_uav_heaps[GPU_Node_Index];
    DX12DescriptorHeap::D3D12DescriptorHeap* sampler_heap = dx12_device->m_descriptor_heap->m_gpu_sampler_heaps[GPU_Node_Index];

    Vector<GPUShaderResource> const& parameter_table_resources = dx12_root_signature->m_tables[info.set_index].resources;

    uint32_t cbv_srv_uav_count = 0;
    uint32_t sampler_count = 0;
    for (GPUShaderResource const& resource : parameter_table_resources)
    {
        if (resource.resource_type == GPUResourceTypeFlag::e_sampler)
            sampler_count++;
        else if (resource.resource_type == GPUResourceTypeFlag::e_texture ||
            resource.resource_type == GPUResourceTypeFlag::e_rw_texture ||
            resource.resource_type == GPUResourceTypeFlag::e_buffer ||
            resource.resource_type == GPUResourceTypeFlag::e_buffer_raw ||
            resource.resource_type == GPUResourceTypeFlag::e_rw_buffer ||
            resource.resource_type == GPUResourceTypeFlag::e_rw_buffer_raw ||
            resource.resource_type == GPUResourceTypeFlag::e_texture_cube ||
            resource.resource_type == GPUResourceTypeFlag::e_uniform_buffer)
            cbv_srv_uav_count += descriptor_count_need(resource);
    }

    // cbv srv uav
    if (cbv_srv_uav_count)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE handle = DX12DescriptorHeap::consume_descriptor_handle(cbv_srv_uav_heap, cbv_srv_uav_count).gpu;
        m_cbv_srv_uav_handle = handle.ptr - cbv_srv_uav_heap->start_handle.gpu.ptr;
        m_cbv_srv_uav_stride = cbv_srv_uav_count * cbv_srv_uav_heap->descriptor_size;
    }
    if (sampler_count)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE handle = DX12DescriptorHeap::consume_descriptor_handle(sampler_heap, cbv_srv_uav_count).gpu;
        m_sampler_handle = handle.ptr - sampler_heap->start_handle.gpu.ptr;
        m_sampler_stride = sampler_count * sampler_heap->descriptor_size;
    }

    // bind null handle on creation
    if (cbv_srv_uav_count || sampler_count)
    {
        uint32_t cbv_srv_uav_heap_index = 0;
        uint32_t sampler_heap_index = 0;
        for (GPUShaderResource const& resource : parameter_table_resources)
        {
            const uint8_t texture_type = to_underlying(resource.texture_type);
            D3D12_CPU_DESCRIPTOR_HANDLE cbv_srv_uav_handle{};
            D3D12_CPU_DESCRIPTOR_HANDLE sampler_handle{};
            switch (static_cast<GPUResourceTypeFlag>(resource.resource_type))
            {
            case GPUResourceTypeFlag::e_texture:
                cbv_srv_uav_handle = dx12_device->m_descriptor_heap->m_null_descriptors->texture_srv[texture_type];
                break;
            case GPUResourceTypeFlag::e_buffer:
                cbv_srv_uav_handle = dx12_device->m_descriptor_heap->m_null_descriptors->buffer_srv;
                break;
            case GPUResourceTypeFlag::e_rw_buffer:
                cbv_srv_uav_handle = dx12_device->m_descriptor_heap->m_null_descriptors->buffer_uav;
                break;
            case GPUResourceTypeFlag::e_uniform_buffer:
                cbv_srv_uav_handle = dx12_device->m_descriptor_heap->m_null_descriptors->buffer_cbv;
                break;
            case GPUResourceTypeFlag::e_sampler:
                sampler_handle = dx12_device->m_descriptor_heap->m_null_descriptors->sampler;
                break;
            default:
                break;
            }
            if (cbv_srv_uav_handle.ptr != 0)
            {
                for (uint32_t j = 0; j < resource.size; j++)
                {
                    DX12DescriptorHeap::copy_descriptor_handle(cbv_srv_uav_heap, cbv_srv_uav_handle, m_cbv_srv_uav_handle, cbv_srv_uav_heap_index);
                    cbv_srv_uav_heap_index++;
                }
            }
            if (sampler_handle.ptr != 0)
            {
                for (uint32_t j = 0; j < resource.size; j++)
                {
                    DX12DescriptorHeap::copy_descriptor_handle(sampler_heap, sampler_handle, m_sampler_handle, sampler_heap_index);
                    sampler_heap_index++;
                }
            }
        }
    }

    // todo: support root descriptors


    m_ref_device = device;
    m_ref_root_signature = info.root_signature;
    m_set_index = info.set_index;

    return AResult::e_succeed;
}

void DX12DescriptorSet::update(Vector<GPUDescriptorData> const& descriptor_data)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(m_ref_device);
    DX12RootSignature const* dx12_root_signature = static_cast<DX12RootSignature const*>(m_ref_root_signature);

    DX12DescriptorHeap::D3D12DescriptorHeap* cbv_srv_uav_heap = dx12_device->m_descriptor_heap->m_gpu_cbv_srv_uav_heaps[GPU_Node_Index];
    DX12DescriptorHeap::D3D12DescriptorHeap* sampler_heap = dx12_device->m_descriptor_heap->m_gpu_sampler_heaps[GPU_Node_Index];

    for (const GPUDescriptorData& data : descriptor_data)
    {
        GPUShaderResource const* shader_resource = nullptr;
        uint32_t heap_offset = 0;
        if (!data.name.empty())
        {
            uint64_t name_hash = hash_str(data.name.c_str(), data.name.size());
            for (const GPUShaderResource& resource : dx12_root_signature->m_tables[m_set_index].resources)
            {
                if (resource.name_hash == name_hash)
                {
                    shader_resource = &resource;
                    break;
                }
                heap_offset += descriptor_count_need(resource);
            }
        }
        else
        {
            for (const GPUShaderResource& resource : dx12_root_signature->m_tables[m_set_index].resources)
            {
                if (resource.resource_type == data.resource_type || resource.binding == data.binding)
                {
                    shader_resource = &resource;
                    break;
                }
                heap_offset += descriptor_count_need(resource);
            }
        }

        // update
        switch (static_cast<GPUResourceTypeFlag>(shader_resource->resource_type))
        {
        case GPUResourceTypeFlag::e_sampler:
        {
            for (uint32_t i = 0; i < data.array_count; i++)
            {
                DX12Sampler const* dx12_sampler = static_cast<DX12Sampler const*>(data.samplers[i]);
                DX12DescriptorHeap::copy_descriptor_handle(sampler_heap, dx12_sampler->m_handle, m_sampler_handle, i + heap_offset);
            }
        }
        break;
        case GPUResourceTypeFlag::e_texture:
        case GPUResourceTypeFlag::e_texture_cube:
        {
            for (uint32_t i = 0; i < data.array_count; i++)
            {
                DX12TextureView const* dx12_texture_view = static_cast<DX12TextureView const*>(data.textures[i]);
                DX12DescriptorHeap::copy_descriptor_handle(cbv_srv_uav_heap, dx12_texture_view->m_srv_uva_handle, m_cbv_srv_uav_handle, i + heap_offset);
            }
        }
        break;
        case GPUResourceTypeFlag::e_rw_texture:
        {
            for (uint32_t i = 0; i < data.array_count; i++)
            {
                DX12TextureView const* dx12_texture_view = static_cast<DX12TextureView const*>(data.textures[i]);
                DX12DescriptorHeap::copy_descriptor_handle(cbv_srv_uav_heap, {dx12_texture_view->m_srv_uva_handle.ptr + dx12_texture_view->m_uav_offset}, m_cbv_srv_uav_handle, i + heap_offset);
            }
        }
        break;
        case GPUResourceTypeFlag::e_uniform_buffer:
        {
            for (uint32_t i = 0; i < data.array_count; i++)
            {
                DX12Buffer const* dx12_buffer = static_cast<DX12Buffer const*>(data.buffers[i]);
                DX12DescriptorHeap::copy_descriptor_handle(cbv_srv_uav_heap, dx12_buffer->m_handle, m_cbv_srv_uav_handle, i + heap_offset);
            }
        }
        break;
        case GPUResourceTypeFlag::e_buffer:
        case GPUResourceTypeFlag::e_buffer_raw:
        {
            for (uint32_t i = 0; i < data.array_count; i++)
            {
                DX12Buffer const* dx12_buffer = static_cast<DX12Buffer const*>(data.buffers[i]);
                DX12DescriptorHeap::copy_descriptor_handle(cbv_srv_uav_heap, {dx12_buffer->m_handle.ptr + dx12_buffer->m_srv_offset}, m_cbv_srv_uav_handle, i + heap_offset);
            }
        }
        break;
        case GPUResourceTypeFlag::e_rw_buffer:
        case GPUResourceTypeFlag::e_rw_buffer_raw:
        {
            for (uint32_t i = 0; i < data.array_count; i++)
            {
                DX12Buffer const* dx12_buffer = static_cast<DX12Buffer const*>(data.buffers[i]);
                DX12DescriptorHeap::copy_descriptor_handle(cbv_srv_uav_heap, {dx12_buffer->m_handle.ptr + dx12_buffer->m_uav_offset}, m_cbv_srv_uav_handle, i + heap_offset);
            }
        }
        break;
        default:
            break;
        }
    }
}


AMAZING_NAMESPACE_END