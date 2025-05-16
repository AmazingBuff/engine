//
// Created by AmazingBuff on 2025/4/30.
//

#pragma once

#include <rendering/rhi/wrapper.h>
#include <io/read.h>
#include <SDL3/SDL.h>
#include <core/math/algebra.h>

using namespace Amazing;

static constexpr uint32_t Width = 960;
static constexpr uint32_t Height = 540;
static constexpr uint32_t Frame_In_Flight = 2;


extern thread_local GPUInstance* t_instance;
extern thread_local GPUDevice* t_device;
extern thread_local GPUSurface* t_surface;
extern thread_local GPUQueue* t_graphics_queue;
extern thread_local GPUCommandPool* t_command_pool[Frame_In_Flight];
extern thread_local GPUCommandBuffer* t_command_buffer[Frame_In_Flight];
extern thread_local GPUSwapChain* t_swap_chain;
extern thread_local GPUFence* t_present_fence[Frame_In_Flight];
extern thread_local GPUSemaphore* t_image_semaphore;
extern thread_local GPUSemaphore* t_present_semaphore;


struct ImageInfo
{
    int width;
    int height;
    int channels;
    Vector<uint8_t> data;
};

void create_api_object(HWND hwnd, HINSTANCE hinstance, GPUBackend backend);
void destroy_api_object();

ImageInfo load_image(const String& file_path);
Vector<char> compile_shader(const Vector<char>& code, const wchar_t* entry, GPUShaderStageFlag stage);
void transfer_buffer_to_texture(GPUBufferToTextureTransferInfo const& info);