//
// Created by AmazingBuff on 2025/5/13.
//

#ifndef VKFRAMEBUFFER_H
#define VKFRAMEBUFFER_H

#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

class VKDevice;

class VKFramebuffer
{
public:
    VKFramebuffer(VKDevice const* device, VulkanFramebufferCreateInfo const& info);
    ~VKFramebuffer();
private:
    VkFramebuffer m_framebuffer;
    VKDevice const* m_ref_device;

    friend class VKPassTable;
};

AMAZING_NAMESPACE_END

#endif //VKFRAMEBUFFER_H
