//
// Created by AmazingBuff on 2025/6/27.
//

#include "render_command.h"
#include "rendering/rhi/wrapper.h"
#include "rendering/graph/resource/render_graph_resources.h"

AMAZING_NAMESPACE_BEGIN

RenderCommand::RenderCommand(RenderDriver const& driver) : m_ref_driver(driver),
    m_ref_pipeline(nullptr), m_frame_index(0), m_frame_count(0) {}

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

void RenderCommand::resource_barrier(RenderGraphResource const* resources, RenderGraphResourceBarrier const* info, uint32_t count) const
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
                .buffer = resources[i].buffer.buffer,
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

void RenderCommand::copy_resource(RenderGraphResource const& src_resource, RenderGraphResource const& dst_resource) const
{
    GPUResourceTransferInfo transfer_info{};

    bool src_is_buffer = true;
    switch (src_resource.resource_type)
    {
    case RenderGraphResourceType::e_buffer:
        transfer_info.src_buffer.buffer = src_resource.buffer.buffer;
        transfer_info.src_buffer.offset = 0;
        break;
    case RenderGraphResourceType::e_image:
        transfer_info.src_texture.texture = src_resource.image.texture;
        transfer_info.src_texture.subresource = {
            .mip_level = 0,
            .base_array_layer = 0,
            .array_layers = 1
        };
        src_is_buffer = false;
        break;
    }

    switch (dst_resource.resource_type)
    {
    case RenderGraphResourceType::e_buffer:
        RENDERING_ASSERT(src_is_buffer == true, "can't copy data to a buffer from a image!");
        transfer_info.dst_buffer.buffer = dst_resource.buffer.buffer;
        transfer_info.dst_buffer.offset = 0;
        transfer_info.dst_buffer.size = dst_resource.buffer.buffer->description()->size;
        transfer_info.type = GPUResourceTransferType::e_buffer_to_buffer;
        break;
    case RenderGraphResourceType::e_image:
        transfer_info.dst_texture.texture = dst_resource.image.texture;
        transfer_info.dst_texture.subresource =  {
            .mip_level = 0,
            .base_array_layer = 0,
            .array_layers = 1
        };
        transfer_info.type = src_is_buffer ? GPUResourceTransferType::e_buffer_to_texture : GPUResourceTransferType::e_texture_to_texture;
        break;
    }

    m_command_buffers[m_frame_index]->transfer_resource(transfer_info);
}


RenderGraphicsCommand::RenderGraphicsCommand(RenderDriver const& driver) : RenderCommand(driver), m_graphics_encoder(nullptr)
{
    m_frame_count = m_ref_driver.m_driver_info.frame_count;
    initialize_command(m_ref_driver.m_graphics_queue);
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

    m_ref_driver.m_graphics_queue->submit(submit_info);

    refresh_frame();
}

void RenderGraphicsCommand::begin_pass(GPUGraphicsPassCreateInfo const& info)
{
    m_graphics_encoder = m_command_buffers[m_frame_index]->begin_graphics_pass(info);
}

void RenderGraphicsCommand::end_pass()
{
    m_command_buffers[m_frame_index]->end_graphics_pass(m_graphics_encoder);
}

void RenderGraphicsCommand::bind_pipeline(RenderGraphPipeline const* pipeline)
{
    m_graphics_encoder->bind_pipeline(pipeline->graphics_pipeline);
    m_ref_pipeline = pipeline;
}

void RenderGraphicsCommand::bind_vertex_buffers(GPUBufferBinding const* bindings, uint32_t count) const
{
    m_graphics_encoder->bind_vertex_buffers(bindings, count);
}

void RenderGraphicsCommand::bind_index_buffer(GPUBufferBinding const& binding) const
{
    m_graphics_encoder->bind_index_buffer(binding);
}

void RenderGraphicsCommand::draw(uint32_t index_count, uint32_t first_index, uint32_t first_vertex) const
{
    m_graphics_encoder->draw_indexed(index_count, first_index, first_vertex);
}


RenderComputeCommand::RenderComputeCommand(RenderDriver const& driver) : RenderCommand(driver), m_compute_encoder(nullptr)
{
    m_frame_count = m_ref_driver.m_driver_info.frame_count;
    initialize_command(m_ref_driver.m_compute_queue);
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

    m_ref_driver.m_compute_queue->submit(submit_info);

    refresh_frame();
}

void RenderComputeCommand::bind_pipeline(RenderGraphPipeline const* pipeline)
{
    m_compute_encoder->bind_pipeline(pipeline->compute_pipeline);
    m_ref_pipeline = pipeline;
}

void RenderComputeCommand::begin_pass(GPUComputePassCreateInfo const& info)
{
    m_compute_encoder = m_command_buffers[m_frame_index]->begin_compute_pass(info);
}

void RenderComputeCommand::end_pass()
{
    m_command_buffers[m_frame_index]->end_compute_pass(m_compute_encoder);
}


AMAZING_NAMESPACE_END
