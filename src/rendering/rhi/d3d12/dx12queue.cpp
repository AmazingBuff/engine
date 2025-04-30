//
// Created by AmazingBuff on 2025/4/16.
//

#include "dx12queue.h"
#include "dx12command_buffer.h"
#include "dx12fence.h"
#include "dx12semaphore.h"
#include "dx12swapchain.h"
#include "utils/dx_macro.h"

AMAZING_NAMESPACE_BEGIN

DX12Queue::DX12Queue() : m_queue(nullptr), m_fence(nullptr) {}

DX12Queue::~DX12Queue()
{
    PLACEMENT_DELETE(DX12Fence, m_fence);
    DX_FREE(m_queue);
}

void DX12Queue::submit(GPUQueueSubmitInfo const& info) const
{
    ID3D12CommandList** commands = static_cast<ID3D12CommandList**>(alloca(info.command_buffers.size() * sizeof(ID3D12CommandList*)));
    for (size_t i = 0; i < info.command_buffers.size(); i++)
        commands[i] = static_cast<DX12CommandBuffer const*>(info.command_buffers[i])->m_command_list;

    // wait
    for (GPUSemaphore const* wait_semaphore : info.wait_semaphores)
    {
        DX12Semaphore const* semaphore = static_cast<DX12Semaphore const*>(wait_semaphore);
        DX_CHECK_RESULT(m_queue->Wait(semaphore->m_fence, semaphore->m_fence_value));
    }
    // execute
    m_queue->ExecuteCommandLists(info.command_buffers.size(), commands);
    // signal
    if (info.signal_fence)
    {
        DX12Fence* fence = static_cast<DX12Fence*>(info.signal_fence);
        DX_CHECK_RESULT(m_queue->Signal(fence->m_fence, ++fence->m_fence_value));
    }
    for (size_t i = 0; i < info.signal_semaphores.size(); i++)
    {
        DX12Semaphore* semaphore = static_cast<DX12Semaphore*>(info.signal_semaphores[i]);
        DX_CHECK_RESULT(m_queue->Signal(semaphore->m_fence, ++semaphore->m_fence_value));
    }
}

void DX12Queue::wait_idle() const
{
    DX_CHECK_RESULT(m_queue->Signal(m_fence->m_fence, ++m_fence->m_fence_value));
    m_fence->wait();
}

void DX12Queue::present(GPUQueuePresentInfo const& info) const
{
    DX12SwapChain const* dx12_swap_chain = static_cast<DX12SwapChain const*>(info.swap_chain);
    DX_CHECK_RESULT(dx12_swap_chain->m_swap_chain->Present(dx12_swap_chain->m_present_sync_interval, dx12_swap_chain->m_present_flags));
}

AMAZING_NAMESPACE_END