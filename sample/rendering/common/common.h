//
// Created by AmazingBuff on 2025/4/30.
//

#pragma once

#include <rendering/rhi/wrapper.h>
#include <io/read.h>
#include <SDL3/SDL.h>

using namespace Amazing;

extern thread_local GPUInstance* t_instance;
extern thread_local GPUDevice* t_device;
extern thread_local GPUSurface* t_surface;
extern thread_local GPUQueue* t_graphics_queue;
extern thread_local GPUCommandPool* t_command_pool;
extern thread_local GPUCommandBuffer* t_command_buffer;
extern thread_local GPUSwapChain* t_swap_chain;
extern thread_local GPUFence* t_present_fence;
extern thread_local GPUSemaphore* t_present_semaphore;

static constexpr uint32_t Width = 960;
static constexpr uint32_t Height = 540;

void create_api_object(HWND hwnd, GPUBackend backend);
void destroy_api_object();