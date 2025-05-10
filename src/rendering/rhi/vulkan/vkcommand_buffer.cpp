//
// Created by AmazingBuff on 25-5-10.
//

#include "vkcommand_buffer.h"
#include "vkdevice.h"
#include "vkcommand_pool.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKCommandBuffer::VKCommandBuffer(GPUCommandPool const* pool, GPUCommandBufferCreateInfo const& info)
{
    VKCommandPool const* vk_command_pool = static_cast<VKCommandPool const*>(pool);
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_command_pool->m_ref_device);

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
    VKDevice const* vk_device = static_cast<VKDevice const*>(vk_command_pool->m_ref_device);
    vk_device->m_device_table.vkFreeCommandBuffers(vk_device->m_device, vk_command_pool->m_pool, 1, &m_command_buffer);
}

void VKCommandBuffer::begin_command()
{

}

void VKCommandBuffer::end_command()
{

}

GPUGraphicsPassEncoder* VKCommandBuffer::begin_graphics_pass(GPUGraphicsPassCreateInfo const& info)
{

    return nullptr;
}

void VKCommandBuffer::end_graphics_pass(GPUGraphicsPassEncoder* encoder)
{

}

void VKCommandBuffer::begin_query(GPUQueryPool const* pool, GPUQueryInfo const& info)
{

}

void VKCommandBuffer::end_query(GPUQueryPool const* pool, GPUQueryInfo const& info)
{

}

void VKCommandBuffer::resource_barrier(GPUResourceBarrierInfo const& info)
{

}

void VKCommandBuffer::transfer_buffer_to_texture(GPUBufferToTextureTransferInfo const& info)
{

}


AMAZING_NAMESPACE_END
