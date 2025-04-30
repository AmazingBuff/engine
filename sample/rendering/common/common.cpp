//
// Created by AmazingBuff on 2025/4/30.
//

#include "common.h"

thread_local GPUInstance* t_instance = nullptr;
thread_local GPUDevice* t_device = nullptr;
thread_local GPUSurface* t_surface = nullptr;
thread_local GPUQueue* t_graphics_queue = nullptr;
thread_local GPUCommandPool* t_command_pool = nullptr;
thread_local GPUCommandBuffer* t_command_buffer = nullptr;
thread_local GPUSwapChain* t_swap_chain = nullptr;
thread_local GPUFence* t_present_fence = nullptr;
thread_local GPUSemaphore* t_present_semaphore = nullptr;

void create_api_object(HWND hwnd, GPUBackend backend)
{
    GPUInstanceCreateInfo instance_create_info{
        .backend = backend,
        .enable_debug_layer = true,
        .enable_gpu_based_validation = true,
    };

    t_instance = GPU_create_instance(instance_create_info);

    Vector<GPUAdapter*> adapters;
    t_instance->enum_adapters(adapters);

    GPUAdapter* const adapter = adapters[0];


    GPUQueueGroup queue_group{
        .queue_type = GPUQueueType::e_graphics,
        .queue_count = 1
    };
    GPUDeviceCreateInfo device_create_info{
        .disable_pipeline_cache = false,
    };

    device_create_info.queue_groups.emplace_back(queue_group);
    t_device = GPU_create_device(adapter, device_create_info);

    t_graphics_queue = (GPUQueue*)t_device->fetch_queue(GPUQueueType::e_graphics, 0);


    t_command_pool = GPU_create_command_pool(t_device, t_graphics_queue);

    GPUCommandBufferCreateInfo command_buffer_create_info{};

    t_command_buffer = GPU_create_command_buffer(t_device, t_command_pool, command_buffer_create_info);


    t_present_fence = GPU_create_fence(t_device);

    t_present_semaphore = GPU_create_semaphore(t_device);

    t_surface = GPU_create_surface(hwnd);

    GPUSwapChainCreateInfo swap_chain_create_info{
        .width = Width,
        .height = Height,
        .frame_count = 3,
        .format = GPUFormat::e_r8g8b8a8_unorm,
        .enable_vsync = true,
        .surface = t_surface,
        .present_queues = { t_graphics_queue }
    };

    t_swap_chain = GPU_create_swap_chain(t_instance, t_device, swap_chain_create_info);
}

void destroy_api_object()
{
    GPU_destroy_fence(t_present_fence);
    GPU_destroy_semaphore(t_present_semaphore);

    GPU_destroy_command_buffer(t_command_buffer);
    GPU_destroy_command_pool(t_command_pool);
    GPU_destroy_swap_chain(t_swap_chain);

    GPU_destroy_surface(t_surface);
    GPU_destroy_device(t_device);
    GPU_destroy_instance(t_instance);
}