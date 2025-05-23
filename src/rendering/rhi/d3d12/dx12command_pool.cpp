//
// Created by AmazingBuff on 2025/4/17.
//

#include "dx12command_pool.h"
#include "dx12device.h"
#include "dx12queue.h"
#include "utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

DX12CommandPool::DX12CommandPool(GPUQueue const* queue, GPUCommandPoolCreateInfo const& info) : m_command_allocator(nullptr)
{
    DX12Queue const* dx12_queue = static_cast<DX12Queue const*>(queue);
    DX12Device const* dx12_device = static_cast<DX12Device const*>(dx12_queue->m_ref_device);

    D3D12_COMMAND_LIST_TYPE type = dx12_queue->m_type == GPUQueueType::e_transfer ? D3D12_COMMAND_LIST_TYPE_COPY :
    (dx12_queue->m_type == GPUQueueType::e_compute ? D3D12_COMMAND_LIST_TYPE_COMPUTE : D3D12_COMMAND_LIST_TYPE_DIRECT);

    DX_CHECK_RESULT(dx12_device->m_device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_command_allocator)));
    m_ref_queue = queue;
}

DX12CommandPool::~DX12CommandPool()
{
    DX_FREE(m_command_allocator);
}

void DX12CommandPool::reset()
{
    DX_CHECK_RESULT(m_command_allocator->Reset());
}



AMAZING_NAMESPACE_END
