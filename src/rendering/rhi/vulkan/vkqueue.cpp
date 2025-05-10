//
// Created by AmazingBuff on 25-5-10.
//

#include "vkqueue.h"
#include "vkdevice.h"
#include "vkfence.h"
#include "vksemaphore.h"
#include "vkcommand_buffer.h"
#include "vkswapchain.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKQueue::VKQueue() : m_queue(nullptr)  {}

VKQueue::~VKQueue() {}

void VKQueue::submit(GPUQueueSubmitInfo const& info) const
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);

    VkCommandBuffer* commands = static_cast<VkCommandBuffer*>(alloca(info.command_buffers.size() * sizeof(VkCommandBuffer)));
    for (size_t i = 0; i < info.command_buffers.size(); i++)
        commands[i] = static_cast<VKCommandBuffer const*>(info.command_buffers[i])->m_command_buffer;

    VkSemaphore* wait_semaphores = info.wait_semaphores.empty() ? nullptr : static_cast<VkSemaphore*>(alloca(info.wait_semaphores.size() * sizeof(VkSemaphore)));
    VkSemaphoreWaitFlagsKHR* wait_flags = info.wait_semaphores.empty() ? nullptr : static_cast<VkSemaphoreWaitFlagsKHR*>(alloca(info.wait_semaphores.size() * sizeof(VkSemaphoreWaitFlagsKHR)));
    for (size_t i = 0; i < info.wait_semaphores.size(); i++)
    {
        wait_semaphores[i] = static_cast<VKSemaphore const*>(info.wait_semaphores[i])->m_semaphore;
        wait_flags[i] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT;
    }

    VkSemaphore* signal_semaphores = info.signal_semaphores.empty() ? nullptr : static_cast<VkSemaphore*>(alloca(info.signal_semaphores.size() * sizeof(VkSemaphore)));
    for (size_t i = 0; i < info.signal_semaphores.size(); i++)
        signal_semaphores[i] = static_cast<VKSemaphore const*>(info.signal_semaphores[i])->m_semaphore;

    VkSubmitInfo submit_info{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = static_cast<uint32_t>(info.wait_semaphores.size()),
        .pWaitSemaphores = wait_semaphores,
        .pWaitDstStageMask = wait_flags,
        .commandBufferCount = static_cast<uint32_t>(info.command_buffers.size()),
        .pCommandBuffers = commands,
        .signalSemaphoreCount = static_cast<uint32_t>(info.signal_semaphores.size()),
        .pSignalSemaphores = signal_semaphores
    };

    VK_CHECK_RESULT(vk_device->m_device_table.vkQueueSubmit(m_queue, 1, &submit_info,
        info.signal_fence ? static_cast<VKFence const*>(info.signal_fence)->m_fence : nullptr));
}

void VKQueue::wait_idle() const
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);
    VK_CHECK_RESULT(vk_device->m_device_table.vkQueueWaitIdle(m_queue));
}

void VKQueue::present(GPUQueuePresentInfo const& info) const
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);
    VKSwapChain const* vk_swap_chain = static_cast<VKSwapChain const*>(info.swap_chain);

    VkSemaphore* wait_semaphores = info.wait_semaphores.empty() ? nullptr : static_cast<VkSemaphore*>(alloca(info.wait_semaphores.size() * sizeof(VkSemaphore)));
    for (size_t i = 0; i < info.wait_semaphores.size(); i++)
        wait_semaphores[i] = static_cast<VKSemaphore const*>(info.wait_semaphores[i])->m_semaphore;

    uint32_t present_index = info.index;
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = static_cast<uint32_t>(info.wait_semaphores.size()),
        .pWaitSemaphores = wait_semaphores,
        .swapchainCount = 1,
        .pSwapchains = &vk_swap_chain->m_swap_chain,
        .pImageIndices = &present_index,
    };

    VkResult result = vk_device->m_device_table.vkQueuePresentKHR(m_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        return;

    VK_CHECK_RESULT(result);
}

AMAZING_NAMESPACE_END