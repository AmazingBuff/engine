//
// Created by AmazingBuff on 2025/5/13.
//

#ifndef VKPASS_TABLE_H
#define VKPASS_TABLE_H

#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

class VKDevice;

class VKPassTable
{
public:
    explicit VKPassTable(VKDevice const* device);
    ~VKPassTable();

    VkRenderPass find_render_pass(VulkanRenderPassCreateInfo const& info);
    VkFramebuffer find_framebuffer(VulkanFramebufferCreateInfo const& info);
private:
    VKDevice const* m_ref_device;

    struct VulkanRenderPassCreateInfoHasher
    {
        size_t operator()(VulkanRenderPassCreateInfo const& render_pass_create_info) const
        {
            return hash_combine(VK_Hash, &render_pass_create_info, sizeof(VulkanRenderPassCreateInfo));
        }
    };

    struct VulkanRenderPassCreateInfoEqual
    {
        bool operator()(VulkanRenderPassCreateInfo const& lhs, VulkanRenderPassCreateInfo const& rhs) const
        {
            if (lhs.color_attachment_count != rhs.color_attachment_count)
                return false;
            return std::memcmp(&lhs, &rhs, sizeof(VulkanRenderPassCreateInfo)) == 0;
        }
    };

    struct VulkanFramebufferCreateInfoHasher
    {
        size_t operator()(VulkanFramebufferCreateInfo const& framebuffer_create_info) const
        {
            return hash_combine(VK_Hash, &framebuffer_create_info, sizeof(VulkanFramebufferCreateInfo));
        }
    };

    struct VulkanFramebufferCreateInfoEqual
    {
        bool operator()(VulkanFramebufferCreateInfo const& lhs, VulkanFramebufferCreateInfo const& rhs) const
        {
            if (lhs.attachment_count != rhs.attachment_count)
                return false;
            return std::memcmp(&lhs, &rhs, sizeof(VulkanFramebufferCreateInfo)) == 0;
        }
    };

    struct VulkanRenderPass
    {
        VkRenderPass render_pass;
    };

    struct VulkanFramebuffer
    {
        VkFramebuffer framebuffer;
    };

    HashMap<VulkanRenderPassCreateInfo, VulkanRenderPass, VulkanRenderPassCreateInfoHasher, VulkanRenderPassCreateInfoEqual>        m_render_pass_map;
    HashMap<VulkanFramebufferCreateInfo, VulkanFramebuffer, VulkanFramebufferCreateInfoHasher, VulkanFramebufferCreateInfoEqual>    m_framebuffer_map;
};

AMAZING_NAMESPACE_END

#endif //VKPASS_TABLE_H
