//
// Created by AmazingBuff on 2025/4/25.
//

#include <rendering/rhi/d3d12/dx12instance.h>
#include <rendering/rhi/d3d12/dx12adapter.h>
#include <rendering/rhi/d3d12/dx12device.h>
#include <rendering/rhi/d3d12/dx12fence.h>
#include <rendering/rhi/d3d12/dx12swapchain.h>

#include <SDL3/SDL.h>

using namespace Amazing;

void create_api_objects(HWND hwnd)
{
    GPUInstanceCreateInfo instance_create_info{
        .backend = GPUBackend::e_d3d12,
        .enable_debug_layer = true,
        .enable_gpu_based_validation = true,
    };

    DX12Instance instance;
    instance.initialize(instance_create_info);

    uint32_t num_adapters = 0;
    instance.enum_adapters(nullptr, &num_adapters);
    DX12Adapter const* adapters = nullptr;
    instance.enum_adapters(reinterpret_cast<GPUAdapter const**>(&adapters), &num_adapters);

    DX12Adapter const& adapter = adapters[0];


    GPUQueueGroup queue_group{
        .queue_type = GPUQueueType::e_graphics,
        .queue_count = 1
    };
    GPUDeviceCreateInfo device_create_info{
        .disable_pipeline_cache = false,
    };

    device_create_info.queue_groups.emplace_back(queue_group);
    DX12Device device;
    device.initialize(reinterpret_cast<GPUAdapter const*>(&adapter), device_create_info);

    GPUQueue const* queue = device.get_queue(GPUQueueType::e_graphics, 0);

    DX12Fence present_fence;
    present_fence.initialize(reinterpret_cast<GPUDevice const*>(&device));

    GPUSurface* surface = reinterpret_cast<GPUSurface*>(hwnd);

    GPUSwapChainCreateInfo swap_chain_create_info{
        .width = 1920,
        .height = 1080,
        .frame_count = 2,
        .format = GPUFormat::e_r8g8b8a8_srgb,
        .enable_vsync = true,
        .surface = surface,
        .present_queues = { queue }
    };

    DX12SwapChain swap_chain;
    swap_chain.initialize(reinterpret_cast<GPUInstance const*>(&instance), reinterpret_cast<GPUDevice const*>(&device), swap_chain_create_info);
}

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return -1;

    SDL_Window* window = SDL_CreateWindow("Amazing Rendering", 1920, 1080, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);

    create_api_objects(hwnd);

    return 0;
}