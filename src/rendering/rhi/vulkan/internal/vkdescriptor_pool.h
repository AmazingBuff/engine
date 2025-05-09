//
// Created by AmazingBuff on 2025/5/7.
//

#ifndef VKDESCRIPTOR_POOL_H
#define VKDESCRIPTOR_POOL_H

#include "rendering/api.h"
#include "rendering/rhi/enumeration.h"
#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

class VKDescriptorPool
{
public:
    VKDescriptorPool(VkDevice device, const VolkDeviceTable* table);
    ~VKDescriptorPool();

private:
    VkDescriptorPool m_pool;
    VkDevice m_ref_device;
    VolkDeviceTable const* m_ref_table;
};

AMAZING_NAMESPACE_END

#endif //VKDESCRIPTOR_POOL_H
