//
// Created by AmazingBuff on 2025/4/15.
//

#include "dx12descriptor_heap.h"
#include "rendering/rhi/d3d12/utils/dx_macro.h"
#include "rendering/rhi/d3d12/utils/dx_utils.h"

AMAZING_NAMESPACE_BEGIN

DX12DescriptorHeap::D3D12DescriptorHeap* DX12DescriptorHeap::create_descriptor_heap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_DESC const& desc)
{
    D3D12DescriptorHeap* heap = Allocator<D3D12DescriptorHeap>::allocate(1);
    DX_CHECK_RESULT(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap->descriptor_heap)));

    heap->start_handle.cpu = heap->descriptor_heap->GetCPUDescriptorHandleForHeapStart();
    if (desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
    {
        heap->start_handle.gpu = heap->descriptor_heap->GetGPUDescriptorHandleForHeapStart();

    }
    else
        heap->start_handle.gpu = {};

    heap->descriptor_size = device->GetDescriptorHandleIncrementSize(desc.Type);
    heap->descriptor_count = desc.NumDescriptors;

    return heap;
}

DX12DescriptorHeap::D3D12DescriptorHandle DX12DescriptorHeap::consume_descriptor_handle(D3D12DescriptorHeap* heap, uint32_t descriptor_count)
{
    if (heap->used_descriptor_count + descriptor_count > heap->descriptor_count)
    {
        // resize

    }

    heap->used_descriptor_count += descriptor_count;

    size_t offset = heap->used_descriptor_count * heap->descriptor_size;
    return { {heap->start_handle.cpu.ptr + offset}, {heap->start_handle.gpu.ptr + offset} };
}

void DX12DescriptorHeap::return_descriptor_handle(D3D12DescriptorHeap* heap, D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32_t descriptor_count)
{
    for (uint32_t i = 0; i < descriptor_count; i++)
    {
        D3D12DescriptorHandle free_handle{
            .cpu = {handle.ptr + i * heap->descriptor_size},
            .gpu = {0}
        };

        heap->free_list.push_back(free_handle);
    }
}

void DX12DescriptorHeap::copy_descriptor_handle(ID3D12Device* device, D3D12DescriptorHeap* heap, D3D12_CPU_DESCRIPTOR_HANDLE const& src_handle, uint64_t const& dst_handle, uint32_t index)
{
    device->CopyDescriptorsSimple(1,
        { heap->start_handle.cpu.ptr + dst_handle + (index * heap->descriptor_size) },
        src_handle, heap->descriptor_heap->GetDesc().Type);
}


DX12DescriptorHeap::DX12DescriptorHeap(ID3D12Device* device) : m_cpu_heaps{}, m_gpu_cbv_srv_uav_heaps{}, m_gpu_sampler_heaps{}, m_null_descriptors(nullptr)
{
    D3D12_FEATURE_DATA_D3D12_OPTIONS options;
    DX_CHECK_RESULT(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options)));

    for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i),
            .NumDescriptors = DX12_Cpu_Descriptor_Heap_Count[i],
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
            .NodeMask = 0,
        };

        m_cpu_heaps[i] = create_descriptor_heap(device, desc);
    }
    // one shader visible heap for each linked node
    for (uint32_t i = 0; i < GPU_Node_Count; ++i)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.NodeMask = 0;

        desc.NumDescriptors = DX12_Gpu_CbvSrvUav_Descriptor_Heap_Count;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        m_gpu_cbv_srv_uav_heaps[i] = create_descriptor_heap(device, desc);

        desc.NumDescriptors = DX12_Gpu_Sampler_Descriptor_Heap_Count;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        m_gpu_sampler_heaps[i] = create_descriptor_heap(device, desc);
    }
    // null
    {
        m_null_descriptors = Allocator<D3D12NullDescriptors>::allocate(1);

        D3D12_SAMPLER_DESC sampler_desc{
            .AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            .AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            .AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        };
        m_null_descriptors->sampler = consume_descriptor_handle(m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER], 1).cpu;
        device->CreateSampler(&sampler_desc, m_null_descriptors->sampler);

        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{
            .Format = DXGI_FORMAT_R8_UINT,
            .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
        };

        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc{
            .Format = DXGI_FORMAT_R8_UINT,
        };

        for (int i = D3D12_SRV_DIMENSION_TEXTURE1D; i < D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE; i++)
        {
            srv_desc.ViewDimension = static_cast<D3D12_SRV_DIMENSION>(i);
            create_srv(device, nullptr, srv_desc, m_null_descriptors->texture_srv[i - 2]);
        }

        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
        create_uav(device, nullptr, nullptr, uav_desc, m_null_descriptors->texture_uav[to_underlying(GPUTextureType::e_1d)]);

        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
        create_uav(device, nullptr, nullptr, uav_desc, m_null_descriptors->texture_uav[to_underlying(GPUTextureType::e_1d_array)]);

        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        create_uav(device, nullptr, nullptr, uav_desc, m_null_descriptors->texture_uav[to_underlying(GPUTextureType::e_2d)]);

        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
        create_uav(device, nullptr, nullptr, uav_desc, m_null_descriptors->texture_uav[to_underlying(GPUTextureType::e_2d_array)]);

        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
        create_uav(device, nullptr, nullptr, uav_desc, m_null_descriptors->texture_uav[to_underlying(GPUTextureType::e_3d)]);

        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        create_srv(device, nullptr, srv_desc, m_null_descriptors->buffer_srv);
        create_uav(device, nullptr, nullptr, uav_desc, m_null_descriptors->buffer_uav);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{
            .BufferLocation = 0,
            .SizeInBytes = 0,
        };
        create_cbv(device, cbv_desc, m_null_descriptors->buffer_cbv);
    }
}

DX12DescriptorHeap::~DX12DescriptorHeap()
{
    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++)
    {
        DX_FREE(m_cpu_heaps[i]->descriptor_heap);
        Allocator<D3D12DescriptorHeap>::deallocate(m_cpu_heaps[i]);
    }
    for (uint32_t i = 0; i < GPU_Node_Count; i++)
    {
        DX_FREE(m_gpu_cbv_srv_uav_heaps[i]->descriptor_heap);
        DX_FREE(m_gpu_sampler_heaps[i]->descriptor_heap);
        Allocator<D3D12DescriptorHeap>::deallocate(m_gpu_cbv_srv_uav_heaps[i]);
        Allocator<D3D12DescriptorHeap>::deallocate(m_gpu_sampler_heaps[i]);
    }

    Allocator<D3D12NullDescriptors>::deallocate(m_null_descriptors);
}

void DX12DescriptorHeap::create_cbv(ID3D12Device* device, D3D12_CONSTANT_BUFFER_VIEW_DESC const& cbv_desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const
{
    if (handle.ptr == 0)
        handle = consume_descriptor_handle(m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV], 1).cpu;
    device->CreateConstantBufferView(&cbv_desc, handle);
}

void DX12DescriptorHeap::create_srv(ID3D12Device* device, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC const& srv_desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const
{
    if (handle.ptr == 0)
        handle = consume_descriptor_handle(m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV], 1).cpu;
    device->CreateShaderResourceView(resource, &srv_desc, handle);
}

void DX12DescriptorHeap::create_uav(ID3D12Device* device, ID3D12Resource* resource, ID3D12Resource* counter_resource, D3D12_UNORDERED_ACCESS_VIEW_DESC const& uav_desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const
{
    if (handle.ptr == 0)
        handle = consume_descriptor_handle(m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV], 1).cpu;
    device->CreateUnorderedAccessView(resource, counter_resource, &uav_desc, handle);
}

void DX12DescriptorHeap::create_rtv(ID3D12Device* device, ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC const& rtv_desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const
{
    if (handle.ptr == 0)
        handle = consume_descriptor_handle(m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV], 1).cpu;
    device->CreateRenderTargetView(resource, &rtv_desc, handle);
}

void DX12DescriptorHeap::create_dsv(ID3D12Device* device, ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC const& dsv_desc, D3D12_CPU_DESCRIPTOR_HANDLE& handle) const
{
    if (handle.ptr == 0)
        handle = consume_descriptor_handle(m_cpu_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV], 1).cpu;
    device->CreateDepthStencilView(resource, &dsv_desc, handle);
}


AMAZING_NAMESPACE_END