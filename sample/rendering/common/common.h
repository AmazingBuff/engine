//
// Created by AmazingBuff on 2025/4/30.
//

#pragma once

#include <SDL3/SDL.h>
#include <renderdoc/renderdoc_app.h>
#include <rendering/rhi/wrapper.h>
#include <io/read.h>
#include <event/sdl_event.h>
#include <camera/orbital_camera.h>

using namespace Amazing;

static constexpr uint32_t Width = 960;
static constexpr uint32_t Height = 540;
static constexpr uint32_t Frame_In_Flight = 3;
static constexpr GPUFormat Backend_Format = GPUFormat::e_r8g8b8a8_unorm;
static constexpr GPUFormat Backend_Depth_Stencil_Format = GPUFormat::e_d32_sfloat_s8_uint;

static thread_local SDLEventCallbackHandler SDL_Event_Callback_Handler;
static thread_local OrbitalCamera Orbital_Camera(Vec3f(0.0f, 0.0f, 0.0f));

extern RENDERDOC_API_1_6_0* Renderdoc_Api;

extern thread_local GPUDevice* t_device;
extern thread_local GPUQueue* t_graphics_queue;
extern thread_local GPUQueue* t_compute_queue;
extern thread_local GPUCommandPool* t_command_pool[Frame_In_Flight];
extern thread_local GPUCommandPool* t_compute_pool[Frame_In_Flight];
extern thread_local GPUCommandBuffer* t_command_buffer[Frame_In_Flight];
extern thread_local GPUCommandBuffer* t_compute_buffer[Frame_In_Flight];
extern thread_local GPUSwapChain* t_swap_chain;
extern thread_local GPUFence* t_present_fence[Frame_In_Flight];
extern thread_local GPUSemaphore* t_image_semaphore;
extern thread_local GPUSemaphore* t_present_semaphore;

// for depth stencil
extern thread_local GPUTexture* t_depth_texture;
extern thread_local GPUTextureView* t_depth_texture_view;


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
Vector<char> compile_shader(const Vector<char>& code, const wchar_t* entry, GPUShaderStage stage);
void transfer_buffer_to_texture(GPUBufferToTextureTransferInfo const& info);