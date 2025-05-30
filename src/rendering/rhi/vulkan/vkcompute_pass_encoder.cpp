//
// Created by AmazingBuff on 2025/5/26.
//

#include "vkcompute_pass_encoder.h"
#include "vkcommand_buffer.h"
#include "vkcommand_pool.h"
#include "vkqueue.h"
#include "vkdevice.h"
#include "vkdescriptor_set.h"
#include "vkroot_signature.h"
#include "vkcompute_pipeline.h"

AMAZING_NAMESPACE_BEGIN

VKComputePassEncoder::VKComputePassEncoder() : m_command(nullptr) {}

void VKComputePassEncoder::bind_descriptor_set(GPUDescriptorSet const* set)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VKDescriptorSet const* vk_descriptor_set = static_cast<VKDescriptorSet const*>(set);
    VKRootSignature const* vk_root_signature = static_cast<VKRootSignature const*>(vk_descriptor_set->m_ref_root_signature);

    vk_device->m_device_table.vkCmdBindDescriptorSets(m_command->m_command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE,
        vk_root_signature->m_pipeline_layout, vk_descriptor_set->m_set_index, 1, &vk_descriptor_set->m_descriptor_set,
        0, nullptr);
}

void VKComputePassEncoder::bind_pipeline(GPUComputePipeline const* pipeline)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    VKComputePipeline const* vk_compute_pipeline = static_cast<VKComputePipeline const*>(pipeline);

    vk_device->m_device_table.vkCmdBindPipeline(m_command->m_command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, vk_compute_pipeline->m_pipeline);
}

void VKComputePassEncoder::set_push_constant(GPURootSignature const* root_signature, String const& name, void const* data)
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

void VKComputePassEncoder::dispatch(uint32_t x, uint32_t y, uint32_t z)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(m_command->m_ref_pool);
    VKQueue const* vk_queue = static_cast<VKQueue const*>(vk_command_pool->m_ref_queue);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_queue->m_ref_device);
    vk_device->m_device_table.vkCmdDispatch(m_command->m_command_buffer, x, y, z);
}

AMAZING_NAMESPACE_END