//
// Created by AmazingBuff on 2025/5/14.
//

#include "vkquery_pool.h"
#include "vkdevice.h"
#include "utils/vk_macro.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKQueryPool::VKQueryPool(GPUDevice const* device, GPUQueryPoolCreateInfo const& info)
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(device);

    VkQueryPoolCreateInfo query_pool_info{
        .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
        .queryType = Query_Type_Map[to_underlying(info.query_type)],
        .queryCount = info.query_count,
    };

    VK_CHECK_RESULT(vk_device->m_device_table.vkCreateQueryPool(vk_device->m_device, &query_pool_info, VK_Allocation_Callbacks_Ptr, &m_pool));

    m_ref_device = device;
    m_query_type = info.query_type;
}

VKQueryPool::~VKQueryPool()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);

    vk_device->m_device_table.vkDestroyQueryPool(vk_device->m_device, m_pool, VK_Allocation_Callbacks_Ptr);
}

AMAZING_NAMESPACE_END