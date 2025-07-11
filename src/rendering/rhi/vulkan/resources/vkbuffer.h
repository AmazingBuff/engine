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

    void map(size_t offset, size_t size, const void* data) const override;
    void unmap() const override;
private:
    VkBuffer m_buffer;
    VmaAllocation m_allocation;

    friend class VKBufferView;
    friend class VKDescriptorSet;
    friend class VKCommandBuffer;
    friend class VKGraphicsPassEncoder;
};

AMAZING_NAMESPACE_END

#endif //VKBUFFER_H
