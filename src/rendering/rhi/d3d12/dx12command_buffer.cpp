//
// Created by AmazingBuff on 2025/4/17.
//

#include "dx12command_buffer.h"
#include "dx12device.h"
#include "dx12command_pool.h"
#include "dx12query_pool.h"
#include "utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

static constexpr D3D12_COMMAND_LIST_TYPE Command_List_Map[GPU_Queue_Type_Count] =
{
    D3D12_COMMAND_LIST_TYPE_DIRECT,
    D3D12_COMMAND_LIST_TYPE_COMPUTE,
    D3D12_COMMAND_LIST_TYPE_COPY
};


DX12CommandBuffer::DX12CommandBuffer() : m_command_list(nullptr), m_bound_descriptor_heaps{}, m_bound_heap_index(0), m_bound_root_signature(nullptr) {}

DX12CommandBuffer::~DX12CommandBuffer()
{
    DX_FREE(m_command_list);
}

AResult DX12CommandBuffer::initialize(GPUDevice const* device, GPUCommandPool const* pool, GPUCommandBufferCreateInfo const& info)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);
    DX12CommandPool const* dx12_command_pool = static_cast<DX12CommandPool const*>(pool);

    m_bound_heap_index = GPU_Node_Index;

    DX_CHECK_RESULT(dx12_device->m_device->CreateCommandList(m_bound_heap_index,
        Command_List_Map[std::to_underlying(dx12_command_pool->m_type)], dx12_command_pool->m_command_allocator,
        nullptr, IID_PPV_ARGS(&m_command_list)));
    DX_CHECK_RESULT(m_command_list->Close());

    m_bound_descriptor_heaps[0] = dx12_device->m_descriptor_heap->m_gpu_cbv_srv_uav_heaps[m_bound_heap_index];
    m_bound_descriptor_heaps[1] = dx12_device->m_descriptor_heap->m_gpu_sampler_heaps[m_bound_heap_index];
    m_ref_pool = dx12_command_pool;

    return AResult::e_succeed;
}

void DX12CommandBuffer::begin_command()
{
    DX12CommandPool const* dx12_command_pool = static_cast<DX12CommandPool const*>(m_ref_pool);

    DX_CHECK_RESULT(m_command_list->Reset(dx12_command_pool->m_command_allocator, nullptr));

    if (dx12_command_pool->m_type != GPUQueueType::e_transfer)
    {
        ID3D12DescriptorHeap* heaps[] = {
            m_bound_descriptor_heaps[0]->descriptor_heap,
            m_bound_descriptor_heaps[1]->descriptor_heap
        };
        m_command_list->SetDescriptorHeaps(2, heaps);
    }

    m_bound_root_signature = nullptr;
}

void DX12CommandBuffer::end_command()
{
    DX_CHECK_RESULT(m_command_list->Close());
}

void DX12CommandBuffer::begin_query(GPUQueryPool const* pool, GPUQueryInfo const& info)
{
    DX12QueryPool const* dx12_query_pool = static_cast<DX12QueryPool const*>(pool);

    switch (dx12_query_pool->m_type)
    {
    case D3D12_QUERY_TYPE_TIMESTAMP:
        break;
    default:
        m_command_list->BeginQuery(dx12_query_pool->m_query_heap, dx12_query_pool->m_type, info.index);
        break;
    }
}

void DX12CommandBuffer::end_query(GPUQueryPool const* pool, GPUQueryInfo const& info)
{
    DX12QueryPool const* dx12_query_pool = static_cast<DX12QueryPool const*>(pool);

    m_command_list->EndQuery(dx12_query_pool->m_query_heap, dx12_query_pool->m_type, info.index);
}

AMAZING_NAMESPACE_END
