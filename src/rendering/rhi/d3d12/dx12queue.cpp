//
// Created by AmazingBuff on 2025/4/16.
//

#include "dx12queue.h"
#include "dx12device.h"
#include "dx12fence.h"

AMAZING_NAMESPACE_BEGIN

DX12Queue::DX12Queue() : m_queue(nullptr), m_fence(nullptr) {}

DX12Queue::~DX12Queue()
{
    Allocator<DX12Fence>::deallocate(m_fence);
    m_queue = nullptr;
}

AResult DX12Queue::initialize(GPUDevice const* device, GPUQueueType type, uint32_t index)
{
    DX12Device const* dx12_device = static_cast<DX12Device const*>(device);
    m_queue = dx12_device->m_command_queues[std::to_underlying(type)][index];
    m_type = type;

    m_fence = Allocator<DX12Fence>::allocate(1);
    m_fence->initialize(device);

    return AResult::e_succeed;
}




AMAZING_NAMESPACE_END