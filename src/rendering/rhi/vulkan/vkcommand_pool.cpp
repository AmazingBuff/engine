//
// Created by AmazingBuff on 25-5-10.
//

#include "vkcommand_pool.h"
#include "vkdevice.h"
#include "vkadapter.h"
#include "vkqueue.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKCommandPool::VKCommandPool(GPUQueue const* queue, GPUCommandPoolCreateInfo const& info) : m_pool(nullptr)
{
    VKQueue const* vk_queue = static_cast<VKQueue const*>(queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(vk_device->m_ref_adapter);

    VkCommandPoolCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        // transient.
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = vk_adapter->m_queue_family_indices[to_underlying(vk_queue->m_type)]
    };

    VK_CHECK_RESULT(vk_device->m_device_table.vkCreateCommandPool(vk_device->m_device, &create_info, &VK_Allocation_Callbacks, &m_pool));
    if (!info.name.empty())
        vk_device->set_debug_name(reinterpret_cast<size_t>(m_pool), VK_OBJECT_TYPE_COMMAND_POOL, info.name);

    m_ref_device = vk_device;
}

VKCommandPool::~VKCommandPool()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);
    vk_device->m_device_table.vkDestroyCommandPool(vk_device->m_device, m_pool, &VK_Allocation_Callbacks);
}

void VKCommandPool::reset()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);
    VK_CHECK_RESULT(vk_device->m_device_table.vkResetCommandPool(vk_device->m_device, m_pool, 0));
}


AMAZING_NAMESPACE_END