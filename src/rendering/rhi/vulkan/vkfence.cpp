//
// Created by AmazingBuff on 25-5-10.
//

#include "vkfence.h"
#include "vkdevice.h"
#include "utils/vk_macro.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKFence::VKFence(GPUDevice const* device) : m_fence(nullptr), m_is_signaled(false)
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(device);

    VkFenceCreateInfo create_info{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    };

    VK_CHECK_RESULT(vk_device->m_device_table.vkCreateFence(vk_device->m_device, &create_info, VK_Allocation_Callbacks_Ptr, &m_fence));

    m_ref_device = device;
}

VKFence::~VKFence()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);
    vk_device->m_device_table.vkDestroyFence(vk_device->m_device, m_fence, VK_Allocation_Callbacks_Ptr);
}

void VKFence::wait()
{
    if (m_is_signaled)
    {
        VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);

        VK_CHECK_RESULT(vk_device->m_device_table.vkWaitForFences(vk_device->m_device, 1, &m_fence, VK_TRUE, UINT64_MAX));
        VK_CHECK_RESULT(vk_device->m_device_table.vkResetFences(vk_device->m_device, 1, &m_fence));
    }
    m_is_signaled = false;
}

AMAZING_NAMESPACE_END