//
// Created by AmazingBuff on 2025/5/8.
//

#ifndef VKBUFFER_H
#define VKBUFFER_H

#include "rendering/rhi/common/buffer.h"
#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

class VKBuffer final : public GPUBuffer
{
public:
    VKBuffer(GPUDevice const* device, GPUBufferCreateInfo const& info);
    ~VKBuffer() override;

    void map(size_t offset, size_t size, const void* data) override;
    void unmap() override;
private:
    VkBuffer m_buffer;
    VkBufferView m_storage_texel_view;
    VkBufferView m_uniform_texel_view;
    VmaAllocation m_allocation;
};

AMAZING_NAMESPACE_END

#endif //VKBUFFER_H
