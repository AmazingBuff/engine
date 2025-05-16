//
// Created by AmazingBuff on 2025/5/13.
//

#include "vkframebuffer.h"
#include "rendering/rhi/vulkan/vkdevice.h"
#include "rendering/rhi/vulkan/utils/vk_macro.h"
#include "rendering/rhi/vulkan/utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKFramebuffer::VKFramebuffer(VKDevice const* device, VulkanFramebufferCreateInfo const& info) : m_framebuffer(nullptr)
{
    VkFramebufferCreateInfo framebuffer_info{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = info.render_pass,
        .attachmentCount = info.attachment_count,
        .pAttachments = info.attachments,
        .width = info.width,
        .height = info.height,
        .layers = info.layers,
    };

    VK_CHECK_RESULT(device->m_device_table.vkCreateFramebuffer(device->m_device, &framebuffer_info, VK_Allocation_Callbacks_Ptr, &m_framebuffer));

    m_ref_device = device;
}

VKFramebuffer::~VKFramebuffer()
{
    m_ref_device->m_device_table.vkDestroyFramebuffer(m_ref_device->m_device, m_framebuffer, VK_Allocation_Callbacks_Ptr);
}

AMAZING_NAMESPACE_END