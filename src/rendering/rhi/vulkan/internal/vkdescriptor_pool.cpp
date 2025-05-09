//
// Created by AmazingBuff on 2025/5/7.
//

#include "vkdescriptor_pool.h"
#include "rendering/rhi/vulkan/vkdevice.h"
#include "rendering/rhi/vulkan/utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKDescriptorPool::VKDescriptorPool(VkDevice device, const VolkDeviceTable* table) : m_pool(nullptr)
{
    VkDescriptorPoolCreateInfo descriptor_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .maxSets = VK_Descriptor_Pool_Max_Sets,
        .poolSizeCount = VK_Descriptor_Pool_Size_Count,
        .pPoolSizes = VK_Descriptor_Pool_Sizes,
    };

    VK_CHECK_RESULT(table->vkCreateDescriptorPool(device, &descriptor_info, &VK_Allocation_Callbacks, &m_pool));
    m_ref_device = device;
    m_ref_table = table;
}

VKDescriptorPool::~VKDescriptorPool()
{
    m_ref_table->vkDestroyDescriptorPool(m_ref_device, m_pool, &VK_Allocation_Callbacks);
}



AMAZING_NAMESPACE_END