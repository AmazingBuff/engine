//
// Created by AmazingBuff on 2025/6/27.
//

#include "render_command.h"
#include "render_driver.h"
#include "rendering/rhi/wrapper.h"
#include "rendering/graph/resource/render_graph_resources.h"

AMAZING_NAMESPACE_BEGIN

void RenderCommand::initialize_command(GPUQueue const* queue)
{
    m_command_pools.resize(m_frame_count);
    m_command_buffers.resize(m_frame_count);
    for (uint32_t i = 0; i < m_frame_count; i++)
    {
        GPUCommandPoolCreateInfo command_pool_create_info{};
        GPUCommandBufferCreateInfo command_buffer_create_info{};
        m_command_pools[i] = GPU_create_command_pool(queue, command_pool_create_info);
        m_command_buffers[i] = GPU_create_command_buffer(m_command_pools[i], command_buffer_create_info);
    }
}

void RenderCommand::begin_frame()
{
    m_command_pools[m_frame_index]->reset();
    m_command_buffers[m_frame_index]->begin_command();
}

void RenderCommand::end_frame()
{
    m_command_buffers[m_frame_index]->end_command();
}

void RenderCommand::refresh_frame()
{
    m_frame_index = (m_frame_index + 1) % m_frame_count;
}

void RenderCommand::resource_barrier(RenderGraphResource const* resources, RenderGraphResourceBarrier const* info, uint32_t count)
{
    uint32_t texture_barrier_count = 0;
    uint32_t buffer_barrier_count = 0;

    for (uint32_t i = 0; i < count; i++)
    {
        switch (resources[i].resource_type)
        {
        case RenderGraphResourceType::e_buffer:
            buffer_barrier_count++;
            break;
        case RenderGraphResourceType::e_image:
            texture_barrier_count++;
            break;
        default:
            break;
        }
    }

    GPUResourceBarrierInfo barrier;
    barrier.buffer_barriers.reserve(buffer_barrier_count);
    barrier.texture_barriers.reserve(texture_barrier_count);

    for (uint32_t i = 0; i < count; i++)
    {
        switch (resources[i].resource_type)
        {
        case RenderGraphResourceType::e_buffer:
        {
            GPUBufferBarrier buffer_barrier{
                .buffer = resources[i].buffer,
                .src_state = info[i].src_state,
                .dst_state = info[i].dst_state,
            };
            barrier.buffer_barriers.push_back(buffer_barrier);
            break;
        }
        case RenderGraphResourceType::e_image:
        {
            GPUTextureBarrier texture_barrier{
                .texture = resources[i].image.texture,
                .src_state = info[i].src_state,
                .dst_state = info[i].dst_state,
            };
            barrier.texture_barriers.push_back(texture_barrier);
            break;
        }
        default:
            break;
        }
    }

    m_command_buffers[m_frame_index]->resource_barrier(barrier);
}


RenderGraphicsCommand::RenderGraphicsCommand(RenderDriver const* driver)
{
    m_frame_count = driver->m_driver_info.frame_count;
    initialize_command(driver->m_graphics_queue);

    m_ref_driver = driver;
}

RenderGraphicsCommand::~RenderGraphicsCommand()
{
    for (uint32_t i = 0; i < m_frame_count; i++)
    {
        GPU_destroy_command_buffer(m_command_buffers[i]);
        GPU_destroy_command_pool(m_command_pools[i]);
    }
}

void RenderGraphicsCommand::submit(RenderCommandSubmitInfo const& info)
{
    GPUQueueSubmitInfo submit_info{
        .command_buffers = {m_command_buffers[m_frame_index]},
        .wait_semaphores = info.wait_semaphores,
        .signal_semaphores = info.signal_semaphores,
        .signal_fence = info.signal_fence,
    };

    m_ref_driver->m_graphics_queue->submit(submit_info);

    refresh_frame();
}


RenderComputeCommand::RenderComputeCommand(RenderDriver const* driver)
{
    m_frame_count = driver->m_driver_info.frame_count;
    initialize_command(driver->m_compute_queue);

    m_ref_driver = driver;
}

RenderComputeCommand::~RenderComputeCommand()
{
    for (uint32_t i = 0; i < m_frame_count; i++)
    {
        GPU_destroy_command_buffer(m_command_buffers[i]);
        GPU_destroy_command_pool(m_command_pools[i]);
    }
}

void RenderComputeCommand::submit(RenderCommandSubmitInfo const& info)
{
    GPUQueueSubmitInfo submit_info{
        .command_buffers = {m_command_buffers[m_frame_index]},
        .wait_semaphores = info.wait_semaphores,
        .signal_semaphores = info.signal_semaphores,
        .signal_fence = info.signal_fence,
    };

    m_ref_driver->m_compute_queue->submit(submit_info);

    refresh_frame();
}


AMAZING_NAMESPACE_END
