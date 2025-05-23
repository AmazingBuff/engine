//
// Created by AmazingBuff on 2025/5/7.
//

#include "vkdescriptor_pool.h"
#include "rendering/rhi/vulkan/vkdevice.h"
#include "rendering/rhi/vulkan/utils/vk_macro.h"
#include "rendering/rhi/vulkan/utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKDescriptorPool::VKDescriptorPool(VKDevice const* device) : m_pool(nullptr)
{
    VkDescriptorPoolCreateInfo descriptor_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = VK_Descriptor_Pool_Max_Sets,
        .poolSizeCount = VK_Descriptor_Pool_Size_Count,
        .pPoolSizes = VK_Descriptor_Pool_Sizes,
    };

    VK_CHECK_RESULT(device->m_device_table.vkCreateDescriptorPool(device->m_device, &descriptor_info, VK_Allocation_Callbacks_Ptr, &m_pool));
    m_ref_device = device;
}

VKDescriptorPool::~VKDescriptorPool()
{
    m_ref_device->m_device_table.vkDestroyDescriptorPool(m_ref_device->m_device, m_pool, VK_Allocation_Callbacks_Ptr);
}

VkDescriptorSet VKDescriptorPool::consume_descriptor_set(VkDescriptorSetLayout const* layouts, uint32_t count) const
{
    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_pool,
        .descriptorSetCount = count,
        .pSetLayouts = layouts
    };

    VkDescriptorSet descriptor_set;
    VK_CHECK_RESULT(m_ref_device->m_device_table.vkAllocateDescriptorSets(m_ref_device->m_device, &alloc_info, &descriptor_set));

    return descriptor_set;
}

void VKDescriptorPool::return_descriptor_set(VkDescriptorSet const* sets, uint32_t count) const
{
    VK_CHECK_RESULT(m_ref_device->m_device_table.vkFreeDescriptorSets(m_ref_device->m_device, m_pool, count, sets));
}



AMAZING_NAMESPACE_END