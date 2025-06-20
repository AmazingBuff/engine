//
// Created by AmazingBuff on 2025/4/30.
//

#include "wrapper.h"

// d3d12
#ifdef _WIN64
#include "d3d12/dx12instance.h"
#include "d3d12/dx12device.h"
#include "d3d12/dx12command_pool.h"
#include "d3d12/dx12command_buffer.h"
#include "d3d12/dx12swapchain.h"
#include "d3d12/dx12graphics_pipeline.h"
#include "d3d12/dx12compute_pipeline.h"
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
#endif

// vulkan
#include "vulkan/vkinstance.h"
#include "vulkan/vkdevice.h"
#include "vulkan/vksurface.h"
#include "vulkan/vkcommand_pool.h"
#include "vulkan/vkcommand_buffer.h"
#include "vulkan/vkswapchain.h"
#include "vulkan/vkgraphics_pipeline.h"
#include "vulkan/vkcompute_pipeline.h"
#include "vulkan/vkfence.h"
#include "vulkan/vksemaphore.h"
#include "vulkan/vkdescriptor_set.h"
#include "vulkan/vkroot_signature.h"
#include "vulkan/vkquery_pool.h"
#include "vulkan/resources/vkbuffer.h"
#include "vulkan/resources/vkmemory_pool.h"
#include "vulkan/resources/vksampler.h"
#include "vulkan/resources/vkshader_library.h"
#include "vulkan/resources/vktexture.h"
#include "vulkan/resources/vktexture_view.h"


AMAZING_NAMESPACE_BEGIN

static GPUBackend t_backend = GPUBackend::e_d3d12;

GPUInstance* GPU_create_instance(GPUInstanceCreateInfo const& info)
{
    GPUInstance* instance = nullptr;
    switch (info.backend)
    {
#ifdef _WIN64
    case GPUBackend::e_d3d12:
        t_backend = GPUBackend::e_d3d12;
        instance = PLACEMENT_NEW(DX12Instance, sizeof(DX12Instance), info);
        break;
#endif
    case GPUBackend::e_vulkan:
        t_backend = GPUBackend::e_vulkan;
        instance = PLACEMENT_NEW(VKInstance, sizeof(VKInstance), info);
        break;
    default:
        RENDERING_LOG_ERROR("unsupported backend type");
        break;
    }

    return instance;
}

GPUSurface* GPU_create_surface(GPUInstance const* instance, void* handle, void* hinstance)
{
#ifdef _WIN64
    switch (t_backend)
    {
    case GPUBackend::e_d3d12:
        return static_cast<GPUSurface*>(handle);
    case GPUBackend::e_vulkan:
        VKSurface* surface = PLACEMENT_NEW(VKSurface, sizeof(VKSurface), instance, handle, hinstance);
        return reinterpret_cast<GPUSurface*>(surface);
    }
#endif
    return nullptr;
}

#ifdef _WIN64
#define D3D_CREATE(dx_type, ...) ret = PLACEMENT_NEW(dx_type, sizeof(dx_type), __VA_ARGS__);
#else
#define D3D_CREATE(dx_type, ...)
#endif

#define GPU_CREATE(type, dx_type, vk_type, ...)                         \
    type* ret = nullptr;                                                \
    switch (t_backend)                                                  \
    {                                                                   \
    case GPUBackend::e_d3d12:                                           \
        D3D_CREATE(dx_type, __VA_ARGS__);                               \
        break;                                                          \
    case GPUBackend::e_vulkan:                                          \
        ret = PLACEMENT_NEW(vk_type, sizeof(vk_type), __VA_ARGS__);     \
        break;                                                          \
    }                                                                   \
    return ret;

GPUDevice* GPU_create_device(GPUAdapter const* adapter, GPUDeviceCreateInfo const& info)
{
    GPU_CREATE(GPUDevice, DX12Device, VKDevice, adapter, info);
}
GPUCommandPool* GPU_create_command_pool(GPUQueue const* queue, GPUCommandPoolCreateInfo const& info)
{
    GPU_CREATE(GPUCommandPool, DX12CommandPool, VKCommandPool, queue, info);
}
GPUCommandBuffer* GPU_create_command_buffer(GPUCommandPool const* pool, GPUCommandBufferCreateInfo const& info)
{
    GPU_CREATE(GPUCommandBuffer, DX12CommandBuffer, VKCommandBuffer, pool, info);
}
GPUSwapChain* GPU_create_swap_chain(GPUDevice const* device, GPUSwapChainCreateInfo const& info)
{
    GPU_CREATE(GPUSwapChain, DX12SwapChain, VKSwapChain, device, info);
}
GPUGraphicsPipeline* GPU_create_graphics_pipeline(GPUGraphicsPipelineCreateInfo const& info)
{
    GPU_CREATE(GPUGraphicsPipeline, DX12GraphicsPipeline, VKGraphicsPipeline, info);
}
GPUComputePipeline* GPU_create_compute_pipeline(GPUComputePipelineCreateInfo const& info)
{
    GPU_CREATE(GPUComputePipeline, DX12ComputePipeline, VKComputePipeline, info);
}
GPUFence* GPU_create_fence(GPUDevice const* device)
{
    GPU_CREATE(GPUFence, DX12Fence, VKFence, device);
}
GPUSemaphore* GPU_create_semaphore(GPUDevice const* device)
{
    GPU_CREATE(GPUSemaphore, DX12Semaphore, VKSemaphore, device);
}
GPUDescriptorSet* GPU_create_descriptor_set(GPUDescriptorSetCreateInfo const& info)
{
    GPU_CREATE(GPUDescriptorSet, DX12DescriptorSet, VKDescriptorSet, info);
}
GPURootSignature* GPU_create_root_signature(GPUDevice const* device, GPURootSignatureCreateInfo const& info)
{
    GPU_CREATE(GPURootSignature, DX12RootSignature, VKRootSignature, device, info);
}
GPUQueryPool* GPU_create_query_pool(GPUDevice const* device, GPUQueryPoolCreateInfo const& info)
{
    GPU_CREATE(GPUQueryPool, DX12QueryPool, VKQueryPool, device, info);
}
GPUTextureView* GPU_create_texture_view(GPUTextureViewCreateInfo const& info)
{
    GPU_CREATE(GPUTextureView, DX12TextureView, VKTextureView, info);
}
GPUTexture* GPU_create_texture(GPUDevice const* device, GPUTextureCreateInfo const& info)
{
    GPU_CREATE(GPUTexture, DX12Texture, VKTexture, device, info);
}
GPUBuffer* GPU_create_buffer(GPUDevice const* device, GPUBufferCreateInfo const& info)
{
    GPU_CREATE(GPUBuffer, DX12Buffer, VKBuffer, device, info);
}
GPUMemoryPool* GPU_create_memory_pool(GPUDevice const* device, GPUMemoryPoolCreateInfo const& info)
{
    GPU_CREATE(GPUMemoryPool, DX12MemoryPool, VKMemoryPool, device, info);
}
GPUSampler* GPU_create_sampler(GPUDevice const* device, GPUSamplerCreateInfo const& info)
{
    GPU_CREATE(GPUSampler, DX12Sampler, VKSampler, device, info);
}
GPUShaderLibrary* GPU_create_shader_library(GPUDevice const* device, GPUShaderLibraryCreateInfo const& info)
{
    GPU_CREATE(GPUShaderLibrary, DX12ShaderLibrary, VKShaderLibrary, device, info);
}

GPURootSignaturePool* GPU_create_root_signature_pool(GPURootSignatureCreateInfo const& info)
{
    GPURootSignaturePool* pool = PLACEMENT_NEW(GPURootSignaturePool, sizeof(GPURootSignaturePool));
    return pool;
}

#undef D3D_CREATE
#undef GPU_CREATE


void GPU_destroy_surface(GPUSurface* surface)
{
    switch (t_backend)
    {
    case GPUBackend::e_vulkan:
        PLACEMENT_DELETE(VKSurface, reinterpret_cast<VKSurface*>(surface));
        break;
    case GPUBackend::e_d3d12:
        break;
    }
}

#define GPU_DESTROY(type, name) void GPU_destroy_##name(type* name) { PLACEMENT_DELETE(type, name); }
GPU_DESTROY(GPUInstance, instance)
GPU_DESTROY(GPUDevice, device)
GPU_DESTROY(GPUCommandPool, command_pool)
GPU_DESTROY(GPUCommandBuffer, command_buffer)
GPU_DESTROY(GPUSwapChain, swap_chain)
GPU_DESTROY(GPUGraphicsPipeline, graphics_pipeline)
GPU_DESTROY(GPUComputePipeline, compute_pipeline)
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