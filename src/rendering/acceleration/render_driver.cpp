//
// Created by AmazingBuff on 2025/6/17.
//

#include "render_driver.h"
#include "rendering/rhi/wrapper.h"

AMAZING_NAMESPACE_BEGIN

RenderDriver::RenderDriver(RenderDriverCreateInfo const& info) : m_driver_info{}
{
    GPUInstanceCreateInfo instance_create_info{
        .backend = info.backend == RenderBackend::e_d3d12 ? GPUBackend::e_d3d12 : GPUBackend::e_vulkan,
#if defined(_DEBUG) || defined(DEBUG)
        .enable_debug_layer = true,
        .enable_gpu_based_validation = true,
#else
        .enable_debug_layer = false,
        .enable_gpu_based_validation = false,
#endif
    };
    m_instance = GPU_create_instance(instance_create_info);

    Vector<GPUAdapter*> adapters;
    m_instance->enum_adapters(adapters);

    // todo: support multiple adapter
    GPUAdapter* const adapter = adapters[0];

    GPUQueueGroup graphics_queue_group{
        .queue_type = GPUQueueType::e_graphics,
        .queue_count = 1
    };
    GPUQueueGroup compute_queue_group{
        .queue_type = GPUQueueType::e_compute,
        .queue_count = 1
    };
    GPUDeviceCreateInfo device_create_info{
        .disable_pipeline_cache = false,
        .queue_groups = {graphics_queue_group, compute_queue_group},
    };

    m_device = GPU_create_device(adapter, device_create_info);

    m_graphics_queue = const_cast<GPUQueue*>(m_device->fetch_queue(GPUQueueType::e_graphics, 0));
    m_compute_queue = const_cast<GPUQueue*>(m_device->fetch_queue(GPUQueueType::e_compute, 0));

    m_command_pools.resize(info.frame_count);
    m_compute_pools.resize(info.frame_count);
    m_command_buffers.resize(info.frame_count);
    m_compute_buffers.resize(info.frame_count);
    for (uint32_t i = 0; i < info.frame_count; i++)
    {
        GPUCommandPoolCreateInfo command_pool_create_info{};
        GPUCommandBufferCreateInfo command_buffer_create_info{};
        m_command_pools[i] = GPU_create_command_pool(m_graphics_queue, command_pool_create_info);
        m_compute_pools[i] = GPU_create_command_pool(m_compute_queue, command_pool_create_info);
        m_command_buffers[i] = GPU_create_command_buffer(m_command_pools[i], command_buffer_create_info);
        m_compute_buffers[i] = GPU_create_command_buffer(m_compute_pools[i], command_buffer_create_info);
    }

    m_driver_info.backend = info.backend;
    m_driver_info.frame_count = info.frame_count;
}

RenderDriver::~RenderDriver()
{
    for (uint32_t i = 0; i < m_driver_info.frame_count; i++)
    {
        GPU_destroy_command_buffer(m_command_buffers[i]);
        GPU_destroy_command_pool(m_command_pools[i]);
        GPU_destroy_command_buffer(m_compute_buffers[i]);
        GPU_destroy_command_pool(m_compute_pools[i]);
    }
    GPU_destroy_device(m_device);
    GPU_destroy_instance(m_instance);
}



AMAZING_NAMESPACE_END