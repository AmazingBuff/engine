//
// Created by AmazingBuff on 2025/4/30.
//

#include "rendering/rhi/wrapper.h"
#include "d3d12/dx12instance.h"
#include "d3d12/dx12device.h"
#include "d3d12/dx12command_pool.h"
#include "d3d12/dx12command_buffer.h"
#include "d3d12/dx12swapchain.h"
#include "d3d12/dx12graphics_pipeline.h"
#include "d3d12/dx12fence.h"
#include "d3d12/dx12semaphore.h"
#include "d3d12/dx12descriptor_set.h"
#include "d3d12/dx12root_signature.h"
#include "d3d12/dx12query_pool.h"
#include "d3d12/resources/dx12buffer.h"
#include "d3d12/resources/dx12memory_pool.h"
#include "d3d12/resources/dx12sampler.h"
#include "d3d12/resources/dx12shader_library.h"
#include "d3d12/resources/dx12texture.h"
#include "d3d12/resources/dx12texture_view.h"


AMAZING_NAMESPACE_BEGIN

static thread_local GPUBackend t_backend = GPUBackend::e_d3d12;

GPUInstance* GPU_create_instance(GPUInstanceCreateInfo const& info)
{
    GPUInstance* instance = nullptr;
    switch (info.backend)
    {
#ifdef _WIN64
    case GPUBackend::e_d3d12:
        t_backend = GPUBackend::e_d3d12;
        instance = PLACEMENT_NEW(DX12Instance, sizeof(DX12Instance), nullptr, info);
        break;
#endif
    case GPUBackend::e_vulkan:
        t_backend = GPUBackend::e_vulkan;
        break;
    default:
        RENDERING_LOG_ERROR("unsupported backend type");
        break;
    }

    return instance;
}

GPUSurface* GPU_create_surface(HWND hwnd)
{
    return reinterpret_cast<GPUSurface*>(hwnd);
}

#define GPU_CREATE(type, dx_type, vk_type, ...)                                     \
    type* ret = nullptr;                                                            \
    switch (t_backend)                                                              \
    {                                                                               \
    case GPUBackend::e_d3d12:                                                       \
        ret = PLACEMENT_NEW(dx_type, sizeof(dx_type), nullptr, __VA_ARGS__);        \
        break;                                                                      \
    case GPUBackend::e_vulkan:                                                      \
        break;                                                                      \
    }                                                                               \
    return ret;

GPUDevice* GPU_create_device(GPUAdapter const* adapter, GPUDeviceCreateInfo const& info)
{
    GPU_CREATE(GPUDevice, DX12Device, VkDevice, adapter, info);
}
GPUCommandPool* GPU_create_command_pool(GPUDevice const* device, GPUQueue const* queue)
{
    GPU_CREATE(GPUCommandPool, DX12CommandPool, VkDevice, device, queue);
}
GPUCommandBuffer* GPU_create_command_buffer(GPUDevice const* device, GPUCommandPool const* pool, GPUCommandBufferCreateInfo const& info)
{
    GPU_CREATE(GPUCommandBuffer, DX12CommandBuffer, VkCommandBuffer, device, pool, info);
}
GPUSwapChain* GPU_create_swap_chain(GPUInstance const* instance, GPUDevice const* device, GPUSwapChainCreateInfo const& info)
{
    GPU_CREATE(GPUSwapChain, DX12SwapChain, VkSwapchainKHR, instance, device, info);
}
GPUGraphicsPipeline* GPU_create_graphics_pipeline(GPUDevice const* device, GPUGraphicsPipelineCreateInfo const& info)
{
    GPU_CREATE(GPUGraphicsPipeline, DX12GraphicsPipeline, VkPipeline, device, info);
}
GPUFence* GPU_create_fence(GPUDevice const* device)
{
    GPU_CREATE(GPUFence, DX12Fence, VkFence, device);
}
GPUSemaphore* GPU_create_semaphore(GPUDevice const* device)
{
    GPU_CREATE(GPUSemaphore, DX12Semaphore, VkSemaphore, device);
}
GPUDescriptorSet* GPU_create_descriptor_set(GPUDevice const* device, GPUDescriptorSetCreateInfo const& info)
{
    GPU_CREATE(GPUDescriptorSet, DX12DescriptorSet, VkDescriptorSet, device, info);
}
GPURootSignature* GPU_create_root_signature(GPUDevice const* device, GPURootSignatureCreateInfo const& info)
{
    GPU_CREATE(GPURootSignature, DX12RootSignature, VkPipelineLayout, device, info);
}
GPUQueryPool* GPU_create_query_pool(GPUDevice const* device, GPUQueryPoolCreateInfo const& info)
{
    GPU_CREATE(GPUQueryPool, DX12QueryPool, VkQueryPool, device, info);
}
GPUTextureView* GPU_create_texture_view(GPUDevice const* device, GPUTextureViewCreateInfo const& info)
{
    GPU_CREATE(GPUTextureView, DX12TextureView, VkTextureView, device, info);
}
GPUTexture* GPU_create_texture(GPUDevice const* device, GPUTextureCreateInfo const& info)
{
    GPU_CREATE(GPUTexture, DX12Texture, VkTexture, device, info);
}
GPUBuffer* GPU_create_buffer(GPUDevice const* device, GPUBufferCreateInfo const& info)
{
    GPU_CREATE(GPUBuffer, DX12Buffer, VkBuffer, device, info);
}
GPUMemoryPool* GPU_create_memory_pool(GPUDevice const* device, GPUMemoryPoolCreateInfo const& info)
{
    GPU_CREATE(GPUMemoryPool, DX12MemoryPool, VkMemoryPool, device, info);
}
GPUSampler* GPU_create_sampler(GPUDevice const* device, GPUSamplerCreateInfo const& info)
{
    GPU_CREATE(GPUSampler, DX12Sampler, VkSampler, device, info);
}
GPUShaderLibrary* GPU_create_shader_library(GPUShaderLibraryCreateInfo const& info)
{
    GPU_CREATE(GPUShaderLibrary, DX12ShaderLibrary, VkShaderLibrary, info);
}

GPURootSignaturePool* GPU_create_root_signature_pool(GPURootSignatureCreateInfo const& info)
{
    GPURootSignaturePool* pool = PLACEMENT_NEW(GPURootSignaturePool, sizeof(GPURootSignaturePool), nullptr);
    return pool;
}

#undef GPU_CREATE


void GPU_destroy_surface(GPUSurface*) {}

#define GPU_DESTROY(type, name) void GPU_destroy_##name(type* name) { PLACEMENT_DELETE(type, name); }
GPU_DESTROY(GPUInstance, instance)
GPU_DESTROY(GPUDevice, device)
GPU_DESTROY(GPUCommandPool, command_pool)
GPU_DESTROY(GPUCommandBuffer, command_buffer)
GPU_DESTROY(GPUSwapChain, swap_chain)
GPU_DESTROY(GPUGraphicsPipeline, graphics_pipeline)
GPU_DESTROY(GPUFence, fence)
GPU_DESTROY(GPUSemaphore, semaphore)
GPU_DESTROY(GPUDescriptorSet, descriptor_set)
GPU_DESTROY(GPURootSignature, root_signature)
GPU_DESTROY(GPUQueryPool, query_pool)
GPU_DESTROY(GPUTextureView, texture_view)
GPU_DESTROY(GPUTexture, texture)
GPU_DESTROY(GPUMemoryPool, memory_pool)
GPU_DESTROY(GPUSampler, sampler)
GPU_DESTROY(GPUShaderLibrary, shader_library)
GPU_DESTROY(GPUBuffer, buffer)
GPU_DESTROY(GPURootSignaturePool, root_signature_pool)
#undef GPU_DESTROY

AMAZING_NAMESPACE_END