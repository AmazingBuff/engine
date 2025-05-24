//
// Created by AmazingBuff on 2025/5/13.
//

#include "vkrender_pass.h"
#include "rendering/rhi/vulkan/vkdevice.h"
#include "rendering/rhi/vulkan/utils/vk_macro.h"
#include "rendering/rhi/vulkan/utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKRenderPass::VKRenderPass(VKDevice const* device, VulkanRenderPassCreateInfo const& info) : m_render_pass(nullptr)
{
    VkAttachmentDescription attachments[2 * GPU_Max_Render_Target + 1]{};
    VkAttachmentReference color_attachment_refs[GPU_Max_Render_Target]{};
    VkAttachmentReference color_resolve_attachment_refs[GPU_Max_Render_Target]{};
    VkAttachmentReference depth_stencil_attachment_ref[1]{};

    uint32_t attachment_count = 0;
    for (uint32_t i = 0; i < info.color_attachment_count; i++)
    {
        // description
        attachments[attachment_count].format = transfer_format(info.color_attachment[i].format);
        attachments[attachment_count].samples = transfer_sample_count(info.sample_count);
        attachments[attachment_count].loadOp = Attachment_Load_Op_Map[to_underlying(info.color_attachment[i].load_action)];
        attachments[attachment_count].storeOp = Attachment_Store_Op_Map[to_underlying(info.color_attachment[i].store_action)];
        attachments[attachment_count].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachments[attachment_count].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        // reference
        color_attachment_refs[i].attachment = attachment_count;
        color_attachment_refs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment_count++;
    }

    bool resolve_attachment = false;
    for (uint32_t i = 0; i < info.color_attachment_count; i++)
    {
        if (info.color_attachment[i].resolve_enable)
        {
            // description
            attachments[attachment_count].format = transfer_format(info.color_attachment[i].format);
            attachments[attachment_count].samples = VK_SAMPLE_COUNT_1_BIT;
            attachments[attachment_count].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachments[attachment_count].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachments[attachment_count].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachments[attachment_count].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            // reference
            color_attachment_refs[i].attachment = attachment_count;
            color_attachment_refs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachment_count++;
            resolve_attachment = true;
        }
        else
            color_resolve_attachment_refs[i].attachment = VK_ATTACHMENT_UNUSED;
    }

    bool depth_stencil_attachment = info.depth_stencil_attachment.depth_stencil_format != GPUFormat::e_undefined;
    if (depth_stencil_attachment)
    {
        attachments[attachment_count].format = transfer_format(info.depth_stencil_attachment.depth_stencil_format);
        attachments[attachment_count].samples = transfer_sample_count(info.sample_count);
        attachments[attachment_count].loadOp = Attachment_Load_Op_Map[to_underlying(info.depth_stencil_attachment.depth_load_action)];
        attachments[attachment_count].storeOp = Attachment_Store_Op_Map[to_underlying(info.depth_stencil_attachment.depth_store_action)];
        attachments[attachment_count].stencilLoadOp = Attachment_Load_Op_Map[to_underlying(info.depth_stencil_attachment.stencil_load_action)];
        attachments[attachment_count].stencilStoreOp = Attachment_Store_Op_Map[to_underlying(info.depth_stencil_attachment.stencil_store_action)];
        attachments[attachment_count].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[attachment_count].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_stencil_attachment_ref[0].attachment = attachment_count;
        depth_stencil_attachment_ref[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachment_count++;
    }

    VkSubpassDescription subpass{
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = info.color_attachment_count,
        .pColorAttachments = color_attachment_refs,
        .pResolveAttachments = resolve_attachment ? color_resolve_attachment_refs : nullptr,
        .pDepthStencilAttachment = depth_stencil_attachment ? depth_stencil_attachment_ref : nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr
    };

    VkRenderPassCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = attachment_count,
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 0,
        .pDependencies = nullptr
    };

    VK_CHECK_RESULT(device->m_device_table.vkCreateRenderPass(device->m_device, &create_info, VK_Allocation_Callbacks_Ptr, &m_render_pass));

    m_ref_device = device;
}

VKRenderPass::~VKRenderPass()
{
    m_ref_device->m_device_table.vkDestroyRenderPass(m_ref_device->m_device, m_render_pass, VK_Allocation_Callbacks_Ptr);
}

AMAZING_NAMESPACE_END