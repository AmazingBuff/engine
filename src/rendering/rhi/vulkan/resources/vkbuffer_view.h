//
// Created by AmazingBuff on 2025/7/7.
//

#ifndef VKBUFFER_VIEW_H
#define VKBUFFER_VIEW_H

#include "rendering/rhi/common/buffer_view.h"
#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

class VKBufferView final : public GPUBufferView
{
public:
    explicit VKBufferView(GPUBufferViewCreateInfo const& info);
    ~VKBufferView() override;
private:
    VkBufferView m_srv_view;
    VkBufferView m_uav_view;

    friend class VKDescriptorSet;
    friend class VKCommandBuffer;
};

AMAZING_NAMESPACE_END

#endif //VKBUFFER_VIEW_H
