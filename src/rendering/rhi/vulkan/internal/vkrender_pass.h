//
// Created by AmazingBuff on 2025/5/13.
//

#ifndef VKRENDER_PASS_H
#define VKRENDER_PASS_H

#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

class VKDevice;

class VKRenderPass
{
public:
    VKRenderPass(VKDevice const* device, VulkanRenderPassCreateInfo const& info);
    ~VKRenderPass();
private:
    VkRenderPass m_render_pass;
    VKDevice const* m_ref_device;

    friend class VKPassTable;
};


AMAZING_NAMESPACE_END
#endif //VKRENDER_PASS_H
