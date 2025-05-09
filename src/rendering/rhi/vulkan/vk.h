//
// Created by AmazingBuff on 2025/5/6.
//

#ifndef VK_H
#define VK_H

#ifdef _WIN64
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "volk/volk.h"
#include "rendering/rhi/create_info.h"

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "vma/vk_mem_alloc.h"

AMAZING_NAMESPACE_BEGIN

static constexpr size_t VK_Hash = hash_str("VK", Rendering_Hash);

static constexpr const char* VK_Validation_Layer_Name = "VK_LAYER_KHRONOS_validation";
static constexpr const char* VK_Instance_Extensions[] =
{
    VK_KHR_SURFACE_EXTENSION_NAME,

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(_MACOS)
    VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
    "VK_EXT_metal_surface",
#ifdef VK_KHR_portability_enumeration
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#endif
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
    VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_GGP)
    VK_GGP_STREAM_DESCRIPTOR_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_VI_NN)
    VK_NN_VI_SURFACE_EXTENSION_NAME,
#endif
#ifdef USE_EXTERNAL_MEMORY_EXTENSIONS
    VK_NV_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
    VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
    VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
    VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME,
#endif
    // To legally use HDR formats
    VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,
    /************************************************************************/
    // VR Extensions
    /************************************************************************/
    VK_KHR_DISPLAY_EXTENSION_NAME,
    VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME,
/************************************************************************/
// Multi GPU Extensions
/************************************************************************/
#if VK_KHR_device_group_creation
    VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
#endif
#ifndef NX64
    /************************************************************************/
    // Property querying extensions
    /************************************************************************/
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
#endif
};
static constexpr const char* VK_Device_Extensions[] =
{
    "VK_KHR_portability_subset",
#if defined(VK_VERSION_1_3)
// VK_GOOGLE_USER_TYPE_EXTENSION_NAME,
// VK_GOOGLE_HLSL_FUNCTIONALITY_1_EXTENSION_NAME,
#endif

#if VK_KHR_depth_stencil_resolve
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
#endif
#if VK_KHR_dynamic_rendering
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
#endif
#if VK_EXT_extended_dynamic_state
    VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
#endif
#if VK_EXT_extended_dynamic_state2
    VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME,
#endif
#if VK_EXT_extended_dynamic_state3
    VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME,
#endif
#if VK_EXT_shader_object
    VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
#endif

    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_MAINTENANCE1_EXTENSION_NAME,
    VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
    VK_EXT_SHADER_SUBGROUP_BALLOT_EXTENSION_NAME,
    VK_EXT_SHADER_SUBGROUP_VOTE_EXTENSION_NAME,
    VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
    VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,

#ifdef USE_EXTERNAL_MEMORY_EXTENSIONS
    VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
    VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
    VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME,
    #if defined(VK_USE_PLATFORM_WIN32_KHR)
    VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
    VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME,
    VK_KHR_EXTERNAL_FENCE_WIN32_EXTENSION_NAME,
    #endif
#endif

#if VK_EXT_shader_object
    VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
#endif

// Debug marker extension in case debug utils is not supported
#ifndef ENABLE_DEBUG_UTILS_EXTENSION
    VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
#endif
#if defined(VK_USE_PLATFORM_GGP)
    VK_GGP_FRAME_TOKEN_EXTENSION_NAME,
#endif

#if VK_KHR_draw_indirect_count
    VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME,
#endif
// Fragment shader interlock extension to be used for ROV type functionality in Vulkan
#if VK_EXT_fragment_shader_interlock
    VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME,
#endif
/************************************************************************/
// NVIDIA Specific Extensions
/************************************************************************/
#ifdef USE_NV_EXTENSIONS
    VK_NVX_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME,
#endif
    /************************************************************************/
    // AMD Specific Extensions
    /************************************************************************/
    VK_AMD_DRAW_INDIRECT_COUNT_EXTENSION_NAME,
    VK_AMD_SHADER_BALLOT_EXTENSION_NAME,
    VK_AMD_GCN_SHADER_EXTENSION_NAME,
/************************************************************************/
// Multi GPU Extensions
/************************************************************************/
#if VK_KHR_device_group
    VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
#endif
    /************************************************************************/
    // Bindless & None Uniform access Extensions
    /************************************************************************/
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
#if VK_KHR_maintenance3 // descriptor indexing depends on this
    VK_KHR_MAINTENANCE3_EXTENSION_NAME,
#endif
#if VK_KHR_buffer_device_address
    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
#endif
#if VK_KHR_synchronization2
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
#endif
#if VK_EXT_descriptor_buffer
    VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
#endif

    /************************************************************************/
    // Descriptor Update Template Extension for efficient descriptor set updates
    /************************************************************************/
    VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME,
    /************************************************************************/
    // RDNA2 Extensions
    /************************************************************************/
#if VK_KHR_create_renderpass2
    VK_KHR_MULTIVIEW_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
#endif
#if VK_KHR_fragment_shading_rate
    VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
#endif
/************************************************************************/
// Raytracing
/************************************************************************/
#ifdef ENABLE_RAYTRACING
    VK_NV_RAY_TRACING_EXTENSION_NAME,
#endif
/************************************************************************/
// YCbCr format support
/************************************************************************/
#if VK_KHR_bind_memory2
    // Requirement for VK_KHR_sampler_ycbcr_conversion
    VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
#endif
#if VK_KHR_sampler_ycbcr_conversion
    VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
#endif
/************************************************************************/
// Nsight Aftermath
/************************************************************************/
#if defined(VK_CAN_USE_NSIGHT_AFTERMATH) && defined(ENABLE_NSIGHT_AFTERMATH)
    VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME,
    VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME,
#endif
    /************************************************************************/
    // PCRTC format support
    /************************************************************************/
#if VK_IMG_format_pvrtc
    VK_IMG_FORMAT_PVRTC_EXTENSION_NAME,
#endif
};

static constexpr float VK_Queue_Priorities[] =
{
    1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f,
    1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f,
    1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f,
    1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f,
};

static constexpr VkDescriptorPoolSize VK_Descriptor_Pool_Sizes[] =
{
    { VK_DESCRIPTOR_TYPE_SAMPLER, 1024 },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 8192 },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1024 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1024 },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1024 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8192 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1024 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1 },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1 },
};
static constexpr uint32_t VK_Descriptor_Pool_Size_Count = array_size(VK_Descriptor_Pool_Sizes);
static constexpr uint32_t VK_Descriptor_Pool_Max_Sets = 8192;
static constexpr uint32_t VK_Sparse_Page_Standard_Size = 65535;


// allocate func
static void vulkan_internal_alloc_notify(
    void*                                       data,
    size_t                                      size,
    VkInternalAllocationType                    allocation_type,
    VkSystemAllocationScope                     allocation_scope)
{

}

static void vulkan_internal_free_notify(
    void*                                       data,
    size_t                                      size,
    VkInternalAllocationType                    allocation_type,
    VkSystemAllocationScope                     allocation_scope)
{

}

static void* vulkan_alloc(
    void*                                       data,
    size_t                                      size,
    size_t                                      alignment,
    VkSystemAllocationScope                     allocation_scope)
{
    return allocate(size, alignment);
}

static void vulkan_free(
    void*                                       data,
    void*                                       memory)
{
    deallocate(memory);
}

static void* vulkan_realloc(
    void*                                       data,
    void*                                       p,
    size_t                                      size,
    size_t                                      alignment,
    VkSystemAllocationScope                     allocation_scope)
{
    return reallocate(p, size, alignment);
}

static constexpr VkAllocationCallbacks VK_Allocation_Callbacks = {
    .pUserData = nullptr,
    .pfnAllocation = &vulkan_alloc,
    .pfnReallocation = &vulkan_realloc,
    .pfnFree = &vulkan_free,
    .pfnInternalAllocation = &vulkan_internal_alloc_notify,
    .pfnInternalFree = &vulkan_internal_free_notify,
};


AMAZING_NAMESPACE_END
#endif //VK_H
