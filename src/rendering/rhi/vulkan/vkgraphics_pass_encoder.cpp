//
// Created by AmazingBuff on 2025/5/13.
//

#include "vkgraphics_pass_encoder.h"
#include "vkcommand_buffer.h"
#include "vkcommand_pool.h"
#include "vkqueue.h"
#include "vkdevice.h"
#include "vkadapter.h"
#include "vkgraphics_pipeline.h"
#include "vkroot_signature.h"
#include "vkdescriptor_set.h"
#include "resources/vkbuffer.h"

AMAZING_NAMESPACE_BEGIN

VKGraphicsPassEncoder::VKGraphicsPassEncoder() : m_command(nullptr) {}

VKGraphicsPassEncoder::~VKGraphicsPassEncoder()
{

}

void VKGraphicsPassEncoder::bind_descriptor_set(GPUDescriptorSet const* set)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VKDescriptorSet const* vk_descriptor_set = static_cast<VKDescriptorSet const*>(set);
    VKRootSignature const* vk_root_signature = static_cast<VKRootSignature const*>(vk_descriptor_set->m_ref_root_signature);

    vk_device->m_device_table.vkCmdBindDescriptorSets(m_command->m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        vk_root_signature->m_pipeline_layout, vk_descriptor_set->m_set_index, 1, &vk_descriptor_set->m_descriptor_set,
        0, nullptr);
}

void VKGraphicsPassEncoder::bind_index_buffer(GPUBufferBinding const& binding)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VKBuffer const* buffer = static_cast<VKBuffer const*>(binding.buffer);

    VkIndexType index_type = sizeof(uint16_t) == binding.stride ? VK_INDEX_TYPE_UINT16 :
        sizeof(uint8_t) == binding.stride ? VK_INDEX_TYPE_UINT8_EXT : VK_INDEX_TYPE_UINT32;

    vk_device->m_device_table.vkCmdBindIndexBuffer(m_command->m_command_buffer, buffer->m_buffer, binding.offset, index_type);
}

void VKGraphicsPassEncoder::bind_pipeline(GPUGraphicsPipeline const* pipeline)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VKGraphicsPipeline const* vk_graphics_pipeline = static_cast<VKGraphicsPipeline const*>(pipeline);

    vk_device->m_device_table.vkCmdBindPipeline(m_command->m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_graphics_pipeline->m_pipeline);
}

void VKGraphicsPassEncoder::bind_vertex_buffers(GPUBufferBinding const* bindings, uint32_t binding_count)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(vk_device->m_ref_adapter);

    uint32_t buffer_count = std::min(binding_count, vk_adapter->m_vulkan_detail.device_properties.limits.maxVertexInputBindings);
    VkBuffer* vertex_buffers = STACK_NEW(VkBuffer, buffer_count);
    VkDeviceSize* offsets = STACK_NEW(VkDeviceSize, buffer_count);
    for (uint32_t i = 0; i < buffer_count; ++i)
    {
        VKBuffer const* buffer = static_cast<VKBuffer const*>(bindings[i].buffer);
        vertex_buffers[i] = buffer->m_buffer;
        offsets[i] = bindings[i].offset;
    }
    vk_device->m_device_table.vkCmdBindVertexBuffers(m_command->m_command_buffer, 0, buffer_count, vertex_buffers, offsets);
}

void VKGraphicsPassEncoder::draw(uint32_t vertex_count, uint32_t first_vertex)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    vk_device->m_device_table.vkCmdDraw(m_command->m_command_buffer, vertex_count, 1, first_vertex, 0);
}

void VKGraphicsPassEncoder::draw_indexed(uint32_t index_count, uint32_t first_index, uint32_t first_vertex)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    vk_device->m_device_table.vkCmdDrawIndexed(m_command->m_command_buffer, index_count, 1, first_index, first_vertex, 0);
}

void VKGraphicsPassEncoder::draw_indexed_instance(uint32_t index_count, uint32_t instance_count, uint32_t first_index, uint32_t first_vertex, uint32_t first_instance)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    vk_device->m_device_table.vkCmdDrawIndexed(m_command->m_command_buffer, index_count, instance_count, first_index, first_vertex, first_instance);
}

void VKGraphicsPassEncoder::draw_instance(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    vk_device->m_device_table.vkCmdDraw(m_command->m_command_buffer, vertex_count, instance_count, first_vertex, first_instance);
}

void VKGraphicsPassEncoder::set_push_constant(GPURootSignature const* root_signature, String const& name, void const* data)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VKRootSignature const* vk_root_signature = static_cast<VKRootSignature const*>(root_signature);

    for (uint32_t i = 0; i < vk_root_signature->m_push_constant_count; i++)
    {
        if (name == vk_root_signature->m_push_constants[i].name)
        {
            vk_device->m_device_table.vkCmdPushConstants(m_command->m_command_buffer, vk_root_signature->m_pipeline_layout,
                vk_root_signature->m_push_constant_ranges[i].stageFlags, 0, vk_root_signature->m_push_constant_ranges[i].size, data);
        }
    }
}

void VKGraphicsPassEncoder::set_scissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VkRect2D scissor = {
        .offset{
            .x = static_cast<int>(x),
            .y = static_cast<int>(y),
        },
        .extent{
            .width = width,
            .height = height,
        }
    };

    vk_device->m_device_table.vkCmdSetScissor(m_command->m_command_buffer, 0, 1, &scissor);
}

void VKGraphicsPassEncoder::set_viewport(float x, float y, float width, float height, float min_depth, float max_depth)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VkViewport viewport{
        .x = x,
        .y = y + height,
        .width = width,
        .height = -height,
        .minDepth = min_depth,
        .maxDepth = max_depth
    };

    vk_device->m_device_table.vkCmdSetViewport(m_command->m_command_buffer, 0, 1, &viewport);
}

AMAZING_NAMESPACE_END