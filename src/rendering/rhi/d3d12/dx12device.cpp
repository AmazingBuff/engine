//
// Created by AmazingBuff on 2025/4/11.
//

#include "dx12device.h"
#include "dx12adapter.h"
#include "dx12queue.h"
#include "dx12fence.h"
#include "resources/dx12memory_pool.h"
#include "internal/dx12descriptor_heap.h"
#include "utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

DX12Device::DX12Device(GPUAdapter const* adapter, GPUDeviceCreateInfo const& info) : m_device(nullptr), m_allocator(nullptr), m_descriptor_heap(nullptr), m_pipeline_library(nullptr), m_memory_pool(nullptr)
{
    DX12Adapter const* dx12_adapter = static_cast<DX12Adapter const*>(adapter);
    DX_CHECK_RESULT(D3D12CreateDevice(dx12_adapter->m_adapter, dx12_adapter->m_feature_level, IID_PPV_ARGS(&m_device)));
    m_ref_adapter = adapter;

    // queues
    for (uint32_t i = 0; i < info.queue_groups.size(); i++)
    {
        const GPUQueueGroup& queue_group = info.queue_groups[i];

        size_t type = to_underlying(queue_group.queue_type);
        m_command_queues[type].resize(queue_group.queue_count);

        for (uint32_t j = 0; j < queue_group.queue_count; j++)
        {
            DX12Queue* queue = PLACEMENT_NEW(DX12Queue, sizeof(DX12Queue));

            D3D12_COMMAND_QUEUE_DESC queue_desc{};
            switch (queue_group.queue_type)
            {
            case GPUQueueType::e_graphics:
                queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
                break;
            case GPUQueueType::e_compute:
                queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
                break;
            case GPUQueueType::e_transfer:
                queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
                break;
            default:
                RENDERING_LOG_ERROR("no suitable queue type");
            }
            queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            DX_CHECK_RESULT(m_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&queue->m_queue)));

            queue->m_type = queue_group.queue_type;
            queue->m_fence = PLACEMENT_NEW(DX12Fence, sizeof(DX12Fence), this);

            m_command_queues[type][j] = queue;
        }
    }

    // d3d12ma
    D3D12MA::ALLOCATION_CALLBACKS allocator_callbacks{
        .pAllocate = [](size_t size, size_t alignment, void* data) { return allocate(size, alignment); },
        .pFree = [](void* p, void*) { deallocate(p); },
        .pPrivateData = nullptr
    };

    D3D12MA::ALLOCATOR_DESC allocator_desc{
        .Flags = D3D12MA::ALLOCATOR_FLAG_MSAA_TEXTURES_ALWAYS_COMMITTED,
        .pDevice = m_device,
        .pAllocationCallbacks = &allocator_callbacks,
        .pAdapter = dx12_adapter->m_adapter,
    };

    DX_CHECK_RESULT(D3D12MA::CreateAllocator(&allocator_desc, &m_allocator));

    // tiled memory pool
    GPUMemoryPoolCreateInfo memory_pool_create_info{
        .type = GPUMemoryPoolType::e_tiled,
        .usage = GPUMemoryUsage::e_gpu_only,
        .block_size = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT * 256,
        .min_block_count = 32,
        .max_block_count = 256,
        .min_allocation_alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
    };

    m_memory_pool = PLACEMENT_NEW(DX12MemoryPool, sizeof(DX12MemoryPool), this, memory_pool_create_info);

    // descriptor heap
    m_descriptor_heap = PLACEMENT_NEW(DX12DescriptorHeap, sizeof(DX12DescriptorHeap), m_device);

    // pipeline cache
    if (!info.disable_pipeline_cache)
    {
        HRESULT result = E_NOTIMPL;
        D3D12_FEATURE_DATA_SHADER_CACHE feature;
        if (SUCCEEDED(m_device->CheckFeatureSupport(D3D12_FEATURE_SHADER_CACHE, &feature, sizeof(feature))) &&
            (feature.SupportFlags & D3D12_SHADER_CACHE_SUPPORT_LIBRARY))
        {
            ID3D12Device1* device1 = nullptr;
            if (SUCCEEDED(m_device->QueryInterface(IID_PPV_ARGS(&device1))))
                result = device1->CreatePipelineLibrary(nullptr, 0, IID_PPV_ARGS(&m_pipeline_library));

            DX_FREE(device1);
        }

        if (FAILED(result))
            RENDERING_LOG_WARNING("enable pipeline cache, but not supported!");
    }
}

DX12Device::~DX12Device()
{
    DX_FREE(m_pipeline_library);

    PLACEMENT_DELETE(DX12MemoryPool, m_memory_pool);
    PLACEMENT_DELETE(DX12DescriptorHeap, m_descriptor_heap);

    DX_FREE(m_allocator);

    for (uint8_t i = 0; i < GPU_Queue_Type_Count; i++)
    {
        for (size_t j = 0; j < m_command_queues[i].size(); j++)
        {
            PLACEMENT_DELETE(DX12Queue, m_command_queues[i][j]);
            m_command_queues[i][j] = nullptr;
        }
    }

    DX_FREE(m_device);
}

GPUQueue const* DX12Device::fetch_queue(GPUQueueType type, uint32_t index) const
{
    return m_command_queues[to_underlying(type)][index];
}

AMAZING_NAMESPACE_END