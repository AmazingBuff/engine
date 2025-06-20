//
// Created by AmazingBuff on 2025/5/7.
//

#ifndef VKDESCRIPTOR_POOL_H
#define VKDESCRIPTOR_POOL_H

#include "rendering/rhi/enumeration.h"
#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

class VKDevice;

class VKDescriptorPool
{
public:
    explicit VKDescriptorPool(VKDevice const* device);
    ~VKDescriptorPool();
    VkDescriptorSet consume_descriptor_set(VkDescriptorSetLayout const* layouts, uint32_t count) const;
    void return_descriptor_set(VkDescriptorSet const* sets, uint32_t count) const;
private:
    VkDescriptorPool m_pool;
    VKDevice const* m_ref_device;
};

AMAZING_NAMESPACE_END

#endif //VKDESCRIPTOR_POOL_H
