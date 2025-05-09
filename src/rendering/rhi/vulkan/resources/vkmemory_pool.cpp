//
// Created by AmazingBuff on 2025/5/9.
//

#include "vkmemory_pool.h"
#include "rendering/rhi/vulkan/vkdevice.h"
#include "rendering/rhi/vulkan/utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKMemoryPool::VKMemoryPool(GPUDevice const* device, GPUMemoryPoolCreateInfo const& info) : m_pool(nullptr)
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(device);

    VmaPoolCreateInfo pool_info = {
        .memoryTypeIndex = 0,
        .flags = 0,
        .blockSize = info.block_size,
        .minBlockCount = info.min_block_count,
        .maxBlockCount = info.max_block_count,
        .minAllocationAlignment = info.min_allocation_alignment,
    };

    VK_CHECK_RESULT(vmaCreatePool(vk_device->m_allocator, &pool_info, &m_pool));

    m_ref_device = device;
}

VKMemoryPool::~VKMemoryPool()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);
    vmaDestroyPool(vk_device->m_allocator, m_pool);
}


AMAZING_NAMESPACE_END