//
// Created by AmazingBuff on 25-5-10.
//

#include "vksemaphore.h"
#include "vkdevice.h"
#include "utils/vk_macro.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKSemaphore::VKSemaphore(GPUDevice const* device) : m_semaphore(nullptr)
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(device);

    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VK_CHECK_RESULT(vk_device->m_device_table.vkCreateSemaphore(vk_device->m_device, &semaphore_info, VK_Allocation_Callbacks_Ptr, &m_semaphore));

    m_ref_device = device;
}

VKSemaphore::~VKSemaphore()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);

    vk_device->m_device_table.vkDestroySemaphore(vk_device->m_device, m_semaphore, VK_Allocation_Callbacks_Ptr);
}

AMAZING_NAMESPACE_END