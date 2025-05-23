//
// Created by AmazingBuff on 25-5-10.
//

#include "vkcommand_buffer.h"
#include "vkdevice.h"
#include "vkadapter.h"
#include "vkcommand_pool.h"
#include "vkquery_pool.h"
#include "vkqueue.h"
#include "vkgraphics_pass_encoder.h"
#include "resources/vkbuffer.h"
#include "resources/vktexture.h"
#include "resources/vktexture_view.h"
#include "internal/vkpass_table.h"
#include "utils/vk_macro.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKCommandBuffer::VKCommandBuffer(GPUCommandPool const* pool, GPUCommandBufferCreateInfo const& info)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);

    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vk_command_pool->m_pool,
        .level = info.is_secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    VK_CHECK_RESULT(vk_device->m_device_table.vkAllocateCommandBuffers(vk_device->m_device, &alloc_info, &m_command_buffer));

    m_ref_pool = pool;
}

VKCommandBuffer::~VKCommandBuffer()
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    vk_device->m_device_table.vkFreeCommandBuffers(vk_device->m_device, vk_command_pool->m_pool, 1, &m_command_buffer);
}

void VKCommandBuffer::begin_command()
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VkCommandBufferBeginInfo begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    VK_CHECK_RESULT(vk_device->m_device_table.vkBeginCommandBuffer(m_command_buffer, &begin_info));
}

void VKCommandBuffer::end_command()
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VK_CHECK_RESULT(vk_device->m_device_table.vkEndCommandBuffer(m_command_buffer));
}

GPUGraphicsPassEncoder* VKCommandBuffer::begin_graphics_pass(GPUGraphicsPassCreateInfo const& info)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    uint32_t width = 0, height = 0;
    VkRenderPass render_pass = nullptr;
    {
        VulkanRenderPassCreateInfo render_pass_info{
            .color_attachment_count = info.color_attachment_count,
            .sample_count = info.sample_count,
        };

        for (uint32_t i = 0; i < info.color_attachment_count; i++)
        {
            VulkanRenderPassCreateInfo::VulkanColorAttachment& color_attachment = render_pass_info.color_attachment[i];
            VKTextureView const* texture_view = static_cast<VKTextureView const*>(info.color_attachments[i].texture_view);
            VKTexture const* texture = static_cast<VKTexture const*>(texture_view->m_ref_texture);

            color_attachment.resolve_enable = info.sample_count > GPUSampleCount::e_1 && info.color_attachments[i].resolve_view != nullptr;
            color_attachment.format = texture->m_info->format;
            color_attachment.load_action = info.color_attachments[i].load;
            color_attachment.store_action = info.color_attachments[i].store;
            width = std::max(width, texture->m_info->width);
            height = std::max(height, texture->m_info->height);
        }

        VulkanRenderPassCreateInfo::VulkanDepthStencilAttachment& depth_stencil_attachment = render_pass_info.depth_stencil_attachment;
        if (info.depth_stencil_attachment)
        {
            VKTextureView const* texture_view = static_cast<VKTextureView const*>(info.depth_stencil_attachment->texture_view);
            VKTexture const* texture = static_cast<VKTexture const*>(texture_view->m_ref_texture);

            depth_stencil_attachment.depth_stencil_format = texture->m_info->format;
            depth_stencil_attachment.depth_load_action = info.depth_stencil_attachment->depth_load;
            depth_stencil_attachment.depth_store_action = info.depth_stencil_attachment->depth_store;
            depth_stencil_attachment.stencil_load_action = info.depth_stencil_attachment->stencil_load;
            depth_stencil_attachment.stencil_store_action = info.depth_stencil_attachment->stencil_store;
        }
        else
        {
            depth_stencil_attachment.depth_stencil_format = GPUFormat::e_undefined;
            depth_stencil_attachment.depth_load_action = GPULoadAction::e_dont_care;
            depth_stencil_attachment.depth_store_action = GPUStoreAction::e_dont_care;
            depth_stencil_attachment.stencil_load_action = GPULoadAction::e_dont_care;
            depth_stencil_attachment.stencil_store_action = GPUStoreAction::e_dont_care;
        }
        render_pass = vk_device->m_pass_table->find_render_pass(vk_device, render_pass_info);
    }

    // framebuffer
    VkFramebuffer framebuffer = nullptr;
    {
        VulkanFramebufferCreateInfo framebuffer_info{
            .render_pass = render_pass,
            .width = width,
            .height = height,
            .layers = 1,
        };

        uint32_t attachment_count = 0;
        for (uint32_t i = 0; i < info.color_attachment_count; i++)
        {
            VKTextureView const* texture_view = static_cast<VKTextureView const*>(info.color_attachments[i].texture_view);
            VKTexture const* texture = static_cast<VKTexture const*>(texture_view->m_ref_texture);

            framebuffer_info.attachments[attachment_count] = texture_view->m_rtv_dsv_view;
            framebuffer_info.layers = std::max(framebuffer_info.layers, texture->m_info->array_layers);
            attachment_count++;
        }

        for (uint32_t i = 0; i < info.color_attachment_count; i++)
        {
            if (info.color_attachments[i].resolve_view && info.sample_count > GPUSampleCount::e_1)
            {
                VKTextureView const* texture_view = static_cast<VKTextureView const*>(info.color_attachments[i].resolve_view);
                framebuffer_info.attachments[attachment_count] = texture_view->m_rtv_dsv_view;
                attachment_count++;
            }
        }

        if (info.depth_stencil_attachment && info.depth_stencil_attachment->texture_view)
        {
            VKTextureView const* texture_view = static_cast<VKTextureView const*>(info.depth_stencil_attachment->texture_view);
            VKTexture const* texture = static_cast<VKTexture const*>(texture_view->m_ref_texture);

            framebuffer_info.attachments[attachment_count] = texture_view->m_rtv_dsv_view;
            framebuffer_info.layers = std::max(framebuffer_info.layers, texture->m_info->array_layers);
            attachment_count++;
        }

        framebuffer_info.attachment_count = attachment_count;
        framebuffer = vk_device->m_pass_table->find_framebuffer(vk_device, framebuffer_info);
    }

    // begin
    VkClearValue clear_values[2 * GPU_Max_Render_Target + 1]{};
    uint32_t clear_value_count = 0;
    for (uint32_t i = 0; i < info.color_attachment_count; i++)
    {
        GPUClearColor const& clear_color = info.color_attachments[i].clear_color;
        if (info.color_attachments[i].load == GPULoadAction::e_clear)
            clear_values[clear_value_count].color = {clear_color.color.r, clear_color.color.g, clear_color.color.b, clear_color.color.a};
        clear_value_count++;
    }

    for (uint32_t i = 0; i < info.color_attachment_count; i++)
    {
        if (info.color_attachments[i].resolve_view && info.sample_count > GPUSampleCount::e_1)
            clear_value_count++;
    }

    if (info.depth_stencil_attachment && info.depth_stencil_attachment->texture_view)
    {
        if (info.depth_stencil_attachment->depth_load == GPULoadAction::e_clear)
            clear_values[clear_value_count].depthStencil = {info.depth_stencil_attachment->clear_color.depth_stencil.depth, info.depth_stencil_attachment->clear_color.depth_stencil.stencil};
        clear_value_count++;
    }

    VkRect2D render_area = {
        .offset{
            .x = 0,
            .y = 0,
        },
        .extent{
            .width = width,
            .height = height,
        }
    };

    VkRenderPassBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = render_pass,
        .framebuffer = framebuffer,
        .renderArea = render_area,
        .clearValueCount = clear_value_count,
        .pClearValues = clear_values,
    };

    vk_device->m_device_table.vkCmdBeginRenderPass(m_command_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

    VKGraphicsPassEncoder* graphics_pass_encoder = PLACEMENT_NEW(VKGraphicsPassEncoder, sizeof(VKGraphicsPassEncoder));
    graphics_pass_encoder->m_command = this;
    return graphics_pass_encoder;
}

void VKCommandBuffer::end_graphics_pass(GPUGraphicsPassEncoder* encoder)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    PLACEMENT_DELETE(VKGraphicsPassEncoder, static_cast<VKGraphicsPassEncoder*>(encoder));
    vk_device->m_device_table.vkCmdEndRenderPass(m_command_buffer);
}

void VKCommandBuffer::begin_query(GPUQueryPool const* pool, GPUQueryInfo const& info)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VKQueryPool const* vk_query_pool = static_cast<VKQueryPool const*>(pool);

    VkPipelineStageFlagBits pipeline_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    if (info.shader_stage == GPUShaderStageFlag::e_vertex)
        pipeline_stage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    if (info.shader_stage == GPUShaderStageFlag::e_tessellation_control)
        pipeline_stage = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
    if (info.shader_stage == GPUShaderStageFlag::e_tessellation_evaluation)
        pipeline_stage = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
    if (info.shader_stage == GPUShaderStageFlag::e_geometry)
        pipeline_stage = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
    if (info.shader_stage == GPUShaderStageFlag::e_fragment)
        pipeline_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    if (info.shader_stage == GPUShaderStageFlag::e_compute)
        pipeline_stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    if (info.shader_stage == GPUShaderStageFlag::e_ray_tracing)
        pipeline_stage = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
    if (info.shader_stage == GPUShaderStageFlag::e_all_graphics)
        pipeline_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    switch (vk_query_pool->m_query_type)
    {
    case GPUQueryType::e_timestamp:
        vk_device->m_device_table.vkCmdWriteTimestamp(m_command_buffer, pipeline_stage, vk_query_pool->m_pool, info.index);
        break;
    default:
        break;
    }
}

void VKCommandBuffer::end_query(GPUQueryPool const* pool, GPUQueryInfo const& info)
{
    begin_query(pool, info);
}

void VKCommandBuffer::resource_barrier(GPUResourceBarrierInfo const& info)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(vk_device->m_ref_adapter);

    VkAccessFlags src_access_flags = 0;
    VkAccessFlags dst_access_flags = 0;
    VkBufferMemoryBarrier* buffer_barrier = info.buffer_barriers.empty() ? nullptr : STACK_NEW(VkBufferMemoryBarrier, info.buffer_barriers.size());
    for (size_t i = 0; i < info.buffer_barriers.size(); i++)
    {
        GPUBufferBarrier const& barrier = info.buffer_barriers[i];
        VKBuffer const* buffer = static_cast<VKBuffer const*>(barrier.buffer);

        buffer_barrier[i].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        buffer_barrier[i].pNext = nullptr;
        buffer_barrier[i].buffer = buffer->m_buffer;
        buffer_barrier[i].size = VK_WHOLE_SIZE;
        buffer_barrier[i].offset = 0;
        if (barrier.queue_acquire)
        {
            buffer_barrier[i].srcQueueFamilyIndex = vk_adapter->m_queue_family_indices[to_underlying(barrier.queue_type)];
            buffer_barrier[i].dstQueueFamilyIndex = vk_adapter->m_queue_family_indices[to_underlying(vk_queue->m_type)];
        }
        else if (barrier.queue_release)
        {
            buffer_barrier[i].srcQueueFamilyIndex = vk_adapter->m_queue_family_indices[to_underlying(vk_queue->m_type)];
            buffer_barrier[i].dstQueueFamilyIndex = vk_adapter->m_queue_family_indices[to_underlying(barrier.queue_type)];
        }
        else
        {
            buffer_barrier[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            buffer_barrier[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        }
        if (barrier.src_state == GPUResourceStateFlag::e_unordered_access &&
            barrier.dst_state == GPUResourceStateFlag::e_unordered_access)
        {
            buffer_barrier[i].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            buffer_barrier[i].dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
        }
        else
        {
            buffer_barrier[i].srcAccessMask = transfer_access_state(barrier.src_state);
            buffer_barrier[i].dstAccessMask = transfer_access_state(barrier.dst_state);
        }
        src_access_flags |= buffer_barrier[i].srcAccessMask;
        dst_access_flags |= buffer_barrier[i].dstAccessMask;
    }

    VkImageMemoryBarrier* image_barrier = info.texture_barriers.empty() ? nullptr : STACK_NEW(VkImageMemoryBarrier, info.texture_barriers.size());
    for (size_t i = 0; i < info.texture_barriers.size(); i++)
    {
        GPUTextureBarrier const& barrier = info.texture_barriers[i];
        VKTexture const* texture = static_cast<VKTexture const*>(barrier.texture);

        image_barrier[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_barrier[i].pNext = nullptr;
        image_barrier[i].image = texture->m_image;
        image_barrier[i].subresourceRange.aspectMask = texture->m_info->aspect_mask;
        image_barrier[i].subresourceRange.baseMipLevel = barrier.subresource_barrier ? barrier.mip_level : 0;
        image_barrier[i].subresourceRange.levelCount = barrier.subresource_barrier ? 1 : VK_REMAINING_MIP_LEVELS;
        image_barrier[i].subresourceRange.baseArrayLayer = barrier.subresource_barrier ? barrier.array_layer : 0;
        image_barrier[i].subresourceRange.layerCount = barrier.subresource_barrier ? 1 : VK_REMAINING_ARRAY_LAYERS;
        if (barrier.queue_acquire && barrier.src_state != GPUResourceStateFlag::e_undefined)
        {
            image_barrier[i].srcQueueFamilyIndex = vk_adapter->m_queue_family_indices[to_underlying(barrier.queue_type)];
            image_barrier[i].dstQueueFamilyIndex = vk_adapter->m_queue_family_indices[to_underlying(vk_queue->m_type)];
        }
        else if (barrier.queue_release && barrier.src_state != GPUResourceStateFlag::e_undefined)
        {
            image_barrier[i].srcQueueFamilyIndex = vk_adapter->m_queue_family_indices[to_underlying(vk_queue->m_type)];
            image_barrier[i].dstQueueFamilyIndex = vk_adapter->m_queue_family_indices[to_underlying(barrier.queue_type)];
        }
        else
        {
            image_barrier[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            image_barrier[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        }

        if (barrier.src_state == GPUResourceStateFlag::e_unordered_access &&
            barrier.dst_state == GPUResourceStateFlag::e_unordered_access)
        {
            image_barrier[i].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            image_barrier[i].dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
            image_barrier[i].oldLayout = VK_IMAGE_LAYOUT_GENERAL;
            image_barrier[i].newLayout = VK_IMAGE_LAYOUT_GENERAL;
        }
        else
        {
            image_barrier[i].srcAccessMask = transfer_access_state(barrier.src_state);
            image_barrier[i].dstAccessMask = transfer_access_state(barrier.dst_state);
            image_barrier[i].oldLayout = transfer_image_layout(barrier.src_state);
            image_barrier[i].newLayout = transfer_image_layout(barrier.dst_state);
        }

        src_access_flags |= image_barrier[i].srcAccessMask;
        dst_access_flags |= image_barrier[i].dstAccessMask;
    }

    if (buffer_barrier || image_barrier)
    {
        vk_device->m_device_table.vkCmdPipelineBarrier(m_command_buffer,
            transfer_pipeline_stage(vk_adapter, src_access_flags, vk_queue->m_type),
            transfer_pipeline_stage(vk_adapter, dst_access_flags, vk_queue->m_type),
            0, 0, nullptr, info.buffer_barriers.size(), buffer_barrier, info.texture_barriers.size(), image_barrier);
    }
}

void VKCommandBuffer::transfer_buffer_to_texture(GPUBufferToTextureTransferInfo const& info)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VKBuffer const* buffer = static_cast<VKBuffer const*>(info.src_buffer);
    VKTexture const* texture = static_cast<VKTexture const*>(info.dst_texture);

    uint32_t width = std::max(1u, texture->m_info->width >> info.dst_texture_subresource.mip_level);
    uint32_t height = std::max(1u, texture->m_info->height >> info.dst_texture_subresource.mip_level);
    uint32_t depth = std::max(1u, texture->m_info->depth >> info.dst_texture_subresource.mip_level);

    VkBufferImageCopy region{
        .bufferOffset = info.src_buffer_offset,
        .bufferRowLength = width,
        .bufferImageHeight = height,
        .imageSubresource{
            .aspectMask = texture->m_info->aspect_mask,
            .mipLevel = info.dst_texture_subresource.mip_level,
            .baseArrayLayer = info.dst_texture_subresource.base_array_layer,
            .layerCount = info.dst_texture_subresource.array_layers
        },
        .imageOffset{
            .x = 0,
            .y = 0,
            .z = 0,
        },
        .imageExtent{
            .width = width,
            .height = height,
            .depth = depth,
        },
    };
    vk_device->m_device_table.vkCmdCopyBufferToImage(m_command_buffer, buffer->m_buffer, texture->m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}


AMAZING_NAMESPACE_END
