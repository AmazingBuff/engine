//
// Created by AmazingBuff on 2025/4/30.
//

#ifndef WRAPPER_H
#define WRAPPER_H

#include "common/instance.h"
#include "common/adapter.h"
#include "common/device.h"
#include "common/queue.h"
#include "common/command_pool.h"
#include "common/command_buffer.h"
#include "common/swapchian.h"
#include "common/fence.h"
#include "common/semaphore.h"
#include "common/graphics_pipeline.h"
#include "common/graphics_pass_encoder.h"
#include "common/query_pool.h"
#include "common/root_signature.h"
#include "common/root_signature_pool.h"
#include "common/descriptor_set.h"
#include "common/buffer.h"
#include "common/memory_pool.h"
#include "common/shader_library.h"
#include "common/sampler.h"
#include "common/texture.h"
#include "common/texture_view.h"

#ifdef _WIN64
#include <windows.h>
#endif 


AMAZING_NAMESPACE_BEGIN

GPUInstance* GPU_create_instance(GPUInstanceCreateInfo const& info);
GPUSurface* GPU_create_surface(GPUInstance const* instance, void* handle); // for windows, handle is a HWND
GPUDevice* GPU_create_device(GPUAdapter const* adapter, GPUDeviceCreateInfo const& info);
GPUCommandPool* GPU_create_command_pool(GPUDevice const* device, GPUQueue const* queue);
GPUCommandBuffer* GPU_create_command_buffer(GPUDevice const* device, GPUCommandPool const* pool, GPUCommandBufferCreateInfo const& info);
GPUSwapChain* GPU_create_swap_chain(GPUInstance const* instance, GPUDevice const* device, GPUSwapChainCreateInfo const& info);
GPUGraphicsPipeline* GPU_create_graphics_pipeline(GPUDevice const* device, GPUGraphicsPipelineCreateInfo const& info);
GPUFence* GPU_create_fence(GPUDevice const* device);
GPUSemaphore* GPU_create_semaphore(GPUDevice const* device);
GPURootSignature* GPU_create_root_signature(GPUDevice const* device, GPURootSignatureCreateInfo const& info);
GPUDescriptorSet* GPU_create_descriptor_set(GPUDevice const* device, GPUDescriptorSetCreateInfo const& info);
GPUQueryPool* GPU_create_query_pool(GPUDevice const* device, GPUQueryPoolCreateInfo const& info);
GPUTextureView* GPU_create_texture_view(GPUDevice const* device, GPUTextureViewCreateInfo const& info);
GPUTexture* GPU_create_texture(GPUDevice const* device, GPUTextureCreateInfo const& info);
GPUBuffer* GPU_create_buffer(GPUDevice const* device, GPUBufferCreateInfo const& info);
GPUMemoryPool* GPU_create_memory_pool(GPUDevice const* device, GPUMemoryPoolCreateInfo const& info);
GPUSampler* GPU_create_sampler(GPUDevice const* device, GPUSamplerCreateInfo const& info);
GPUShaderLibrary* GPU_create_shader_library(GPUShaderLibraryCreateInfo const& info);
GPURootSignaturePool* GPU_create_root_signature_pool(GPURootSignatureCreateInfo const& info);


void GPU_destroy_instance(GPUInstance* instance);
void GPU_destroy_surface(GPUSurface* surface);
void GPU_destroy_device(GPUDevice* device);
void GPU_destroy_command_pool(GPUCommandPool* command_pool);
void GPU_destroy_command_buffer(GPUCommandBuffer* command_buffer);
void GPU_destroy_swap_chain(GPUSwapChain* swap_chain);
void GPU_destroy_graphics_pipeline(GPUGraphicsPipeline* graphic_pipeline);
void GPU_destroy_fence(GPUFence* fence);
void GPU_destroy_semaphore(GPUSemaphore* semaphore);
void GPU_destroy_descriptor_set(GPUDescriptorSet* descriptor_set);
void GPU_destroy_root_signature(GPURootSignature* root_signature);
void GPU_destroy_query_pool(GPUQueryPool* query_pool);
void GPU_destroy_buffer(GPUBuffer* buffer);
void GPU_destroy_shader_library(GPUShaderLibrary* shader_library);
void GPU_destroy_memory_pool(GPUMemoryPool* memory_pool);
void GPU_destroy_sampler(GPUSampler* sampler);
void GPU_destroy_texture(GPUTexture* texture);
void GPU_destroy_texture_view(GPUTextureView* texture_view);
void GPU_destroy_root_signature_pool(GPURootSignaturePool* root_signature_pool);

AMAZING_NAMESPACE_END

#endif //WRAPPER_H
