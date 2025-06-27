//
// Created by AmazingBuff on 2025/6/27.
//

#include "render_command.h"
#include "render_driver.h"
#include "rendering/rhi/wrapper.h"

AMAZING_NAMESPACE_BEGIN

RenderCommand::RenderCommand(RenderDriver const* driver)
{
    m_graphics_queue = const_cast<GPUQueue*>(driver->m_device->fetch_queue(GPUQueueType::e_graphics, 0));
    m_compute_queue = const_cast<GPUQueue*>(driver->m_device->fetch_queue(GPUQueueType::e_compute, 0));

    m_command_pools.resize(driver->m_driver_info.frame_count);
    m_compute_pools.resize(driver->m_driver_info.frame_count);
    m_command_buffers.resize(driver->m_driver_info.frame_count);
    m_compute_buffers.resize(driver->m_driver_info.frame_count);
    for (uint32_t i = 0; i < driver->m_driver_info.frame_count; i++)
    {
        GPUCommandPoolCreateInfo command_pool_create_info{};
        GPUCommandBufferCreateInfo command_buffer_create_info{};
        m_command_pools[i] = GPU_create_command_pool(m_graphics_queue, command_pool_create_info);
        m_compute_pools[i] = GPU_create_command_pool(m_compute_queue, command_pool_create_info);
        m_command_buffers[i] = GPU_create_command_buffer(m_command_pools[i], command_buffer_create_info);
        m_compute_buffers[i] = GPU_create_command_buffer(m_compute_pools[i], command_buffer_create_info);
    }
}

RenderCommand::~RenderCommand()
{
    uint32_t frame_count = m_command_buffers.size();
    for (uint32_t i = 0; i < frame_count; i++)
    {
        GPU_destroy_command_buffer(m_command_buffers[i]);
        GPU_destroy_command_pool(m_command_pools[i]);
        GPU_destroy_command_buffer(m_compute_buffers[i]);
        GPU_destroy_command_pool(m_compute_pools[i]);
    }
}

AMAZING_NAMESPACE_END
