//
// Created by AmazingBuff on 2025/5/6.
//

#include "vkadapter.h"
#include "utils/vk_utils.h"
#include "rendering/api.h"

AMAZING_NAMESPACE_BEGIN

static uint32_t combine_version(uint32_t v0, uint32_t v1)
{
    uint32_t multiplier = 1;
    while (multiplier < v1)
        multiplier *= 10;
    return multiplier * v0 + v1;
}

VKAdapter::VKAdapter() : m_vulkan_detail{}, m_physical_device(nullptr), m_queue_family_indices{}
{
    memset(m_queue_family_indices, std::numeric_limits<uint32_t>::max(), sizeof(m_queue_family_indices));
}

VKAdapter::~VKAdapter()
{

}

void VKAdapter::record_adapter_detail()
{
    // query indices
    Vector<VkQueueFamilyProperties> queue_families = enumerate_properties(vkGetPhysicalDeviceQueueFamilyProperties, m_physical_device);
    for (uint32_t j = 0; j < queue_families.size(); j++)
    {
        if (m_queue_family_indices[to_underlying(GPUQueueType::e_graphics)] != std::numeric_limits<uint32_t>::max() &&
            queue_families[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            m_queue_family_indices[to_underlying(GPUQueueType::e_graphics)] = j;
        else if (m_queue_family_indices[to_underlying(GPUQueueType::e_compute)] != std::numeric_limits<uint32_t>::max() &&
            queue_families[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
            m_queue_family_indices[to_underlying(GPUQueueType::e_compute)]= j;
        else if (m_queue_family_indices[to_underlying(GPUQueueType::e_transfer)] != std::numeric_limits<uint32_t>::max() &&
            queue_families[j].queueFlags & VK_QUEUE_TRANSFER_BIT)
            m_queue_family_indices[to_underlying(GPUQueueType::e_transfer)] = j;
    }

    // format support
    for (uint8_t j = 0; j < GPU_Format_Count; j++)
    {
        VkFormatProperties format_properties;
        VkFormat format = transfer_format(static_cast<GPUFormat>(j));
        if (format != VK_FORMAT_UNDEFINED)
        {
            vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &format_properties);
            m_adapter_detail.format_support[j].shader_read = (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0;
            m_adapter_detail.format_support[j].shader_write = (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) != 0;
            m_adapter_detail.format_support[j].render_target_write = (format_properties.optimalTilingFeatures & (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) != 0;
        }
        else
        {
            m_adapter_detail.format_support[j].shader_read = 0;
            m_adapter_detail.format_support[j].shader_write = 0;
            m_adapter_detail.format_support[j].render_target_write = 0;
        }
    }

    // properties
    VkPhysicalDeviceProperties2 properties_2 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2
    };

    VkPhysicalDeviceSubgroupProperties subgroup_properties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES
    };
    properties_2.pNext = &subgroup_properties;
#if VK_KHR_depth_stencil_resolve
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR depth_stencil_resolve_properties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR
    };
    subgroup_properties.pNext = &depth_stencil_resolve_properties;
#endif
#if VK_EXT_extended_dynamic_state3
    VkPhysicalDeviceExtendedDynamicState3PropertiesEXT dynamic_state_properties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_PROPERTIES_EXT
    };
    depth_stencil_resolve_properties.pNext = &dynamic_state_properties;
#endif
#if VK_EXT_shader_object
    VkPhysicalDeviceShaderObjectPropertiesEXT shader_object_properties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_PROPERTIES_EXT
    };
    dynamic_state_properties.pNext = &shader_object_properties;
#endif
#if VK_EXT_descriptor_buffer
    VkPhysicalDeviceDescriptorBufferPropertiesEXT descriptor_buffer_properties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT
    };
    shader_object_properties.pNext = &descriptor_buffer_properties;
#endif
#if VK_KHR_fragment_shading_rate
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragment_shading_rate_properties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR
    };
    descriptor_buffer_properties.pNext = &fragment_shading_rate_properties;
#endif
    vkGetPhysicalDeviceProperties2(m_physical_device, &properties_2);

    // features
    VkPhysicalDeviceFeatures2 features_2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2
    };
#if VK_KHR_buffer_device_address
    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR buffer_device_address_features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR
    };
    features_2.pNext = &buffer_device_address_features;
#endif
#if VK_EXT_descriptor_buffer
    VkPhysicalDeviceDescriptorBufferFeaturesEXT descriptor_buffer_features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT
    };
    buffer_device_address_features.pNext = &descriptor_buffer_features;
#endif
#if VK_KHR_fragment_shading_rate
    VkPhysicalDeviceFragmentShadingRateFeaturesKHR fragment_shading_rate_features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR
    };
    descriptor_buffer_features.pNext = &fragment_shading_rate_features;
#endif
#if VK_KHR_dynamic_rendering
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR
    };
    fragment_shading_rate_features.pNext = &dynamic_rendering_features;
#endif
#if VK_EXT_extended_dynamic_state
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamic_state_features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT
    };
    dynamic_rendering_features.pNext = &dynamic_state_features;
#endif
#if VK_EXT_extended_dynamic_state2
    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT dynamic_state_2_features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT
    };
    dynamic_state_features.pNext = &dynamic_state_2_features;
#endif
#if VK_EXT_extended_dynamic_state3
    VkPhysicalDeviceExtendedDynamicState3FeaturesEXT dynamic_state_3_features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT
    };
    dynamic_state_2_features.pNext = &dynamic_state_3_features;
#endif
#if VK_EXT_shader_object
    VkPhysicalDeviceShaderObjectFeaturesEXT shader_object_features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT
    };
    dynamic_state_3_features.pNext = &shader_object_features;
#endif
    vkGetPhysicalDeviceFeatures2(m_physical_device, &features_2);

    // detail
    const VkPhysicalDeviceProperties& properties = properties_2.properties;
    const VkPhysicalDeviceFeatures& features = features_2.features;
    m_vulkan_detail.device_properties = properties;

    m_adapter_detail.is_cpu = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU;
    m_adapter_detail.is_virtual = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
    m_adapter_detail.is_uma = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;

    m_adapter_detail.vendor.device_id = properties.deviceID;
    m_adapter_detail.vendor.vendor_id = properties.vendorID;
    if (m_adapter_detail.vendor.vendor_id == 0x10DE) // NVIDIA
    {
        const uint32_t vraw = properties.driverVersion;
        const uint32_t v0 = (vraw >> 22) & 0x3ff;
        const uint32_t v1 = (vraw >> 14) & 0x0ff;
        const uint32_t v2 = (vraw >> 6) & 0x0ff;
        const uint32_t v3 = (vraw) & 0x03f;
        m_adapter_detail.vendor.driver_version = combine_version(combine_version(combine_version(v0, v1), v2), v3);
    }
    else if (m_adapter_detail.vendor.vendor_id == 0x8086 ) // Intel
    {
        const uint32_t vraw = properties.driverVersion;
        const uint32_t v0 = (vraw >> 14);
        const uint32_t v1 = (vraw) & 0x3fff;
        m_adapter_detail.vendor.driver_version = combine_version(v0, v1);
    }
    else
        m_adapter_detail.vendor.driver_version = properties.driverVersion;

    memcpy(m_adapter_detail.vendor.gpu_name, properties.deviceName, GPU_Vendor_String_Length);

    m_adapter_detail.uniform_buffer_alignment = properties.limits.minUniformBufferOffsetAlignment;
    m_adapter_detail.upload_buffer_texture_alignment = properties.limits.optimalBufferCopyOffsetAlignment;
    m_adapter_detail.upload_buffer_texture_row_alignment = properties.limits.optimalBufferCopyRowPitchAlignment;
    m_adapter_detail.max_vertex_input_bindings = properties.limits.maxVertexInputBindings;
    m_adapter_detail.multidraw_indirect = properties.limits.maxDrawIndirectCount > 1;
    m_adapter_detail.wave_lane_count = subgroup_properties.subgroupSize;
    m_adapter_detail.support_geom_shader = features.geometryShader;
    m_adapter_detail.support_tessellation = features.tessellationShader;
#if VK_KHR_fragment_shading_rate
    m_adapter_detail.support_shading_rate = fragment_shading_rate_features.pipelineFragmentShadingRate;
    m_adapter_detail.support_shading_rate_mask = fragment_shading_rate_features.attachmentFragmentShadingRate;
    m_adapter_detail.support_shading_rate_sv = fragment_shading_rate_features.primitiveFragmentShadingRate;
#endif
#if VK_EXT_extended_dynamic_state
    if (dynamic_state_features.extendedDynamicState)
        m_adapter_detail.dynamic_states |= GPUDynamicStateFlag::e_tier1;
#endif
#define DYNAMIC_STATES_ADD(feature, flag) m_adapter_detail.dynamic_states |= feature ? flag : GPUDynamicStateFlag::e_undefined;
#if VK_EXT_extended_dynamic_state2
    if (dynamic_state_2_features.extendedDynamicState2)
    {
        m_adapter_detail.dynamic_states |= GPUDynamicStateFlag::e_raster_discard;
        m_adapter_detail.dynamic_states |= GPUDynamicStateFlag::e_depth_bias;
        m_adapter_detail.dynamic_states |= GPUDynamicStateFlag::e_primitive_restart;
    }

    DYNAMIC_STATES_ADD(dynamic_state_2_features.extendedDynamicState2LogicOp, GPUDynamicStateFlag::e_logic_op);
    DYNAMIC_STATES_ADD(dynamic_state_2_features.extendedDynamicState2PatchControlPoints, GPUDynamicStateFlag::e_patch_control_points);
#endif
#if VK_EXT_extended_dynamic_state3
    DYNAMIC_STATES_ADD(dynamic_state_3_features.extendedDynamicState3TessellationDomainOrigin, GPUDynamicStateFlag::e_tessellation_domain_origin);
    DYNAMIC_STATES_ADD(dynamic_state_3_features.extendedDynamicState3DepthClampEnable, GPUDynamicStateFlag::e_depth_clip_enable);
    // todo: add more dynamic state

#endif
#undef DYNAMIC_STATES_ADD

    // memory info
#ifdef VK_EXT_memory_budget
    if (vkGetPhysicalDeviceMemoryProperties2KHR)
    {
        VkPhysicalDeviceMemoryBudgetPropertiesEXT budget_properties{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT
        };
        VkPhysicalDeviceMemoryProperties2 memory_properties_2{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
            .pNext = &budget_properties,
        };
        vkGetPhysicalDeviceMemoryProperties2KHR(m_physical_device, &memory_properties_2);
        const VkPhysicalDeviceMemoryProperties& memory_properties = memory_properties_2.memoryProperties;
        for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
        {
            const uint32_t heap_index = memory_properties.memoryTypes[i].heapIndex;
            if (memory_properties.memoryHeaps[heap_index].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            {
                if ((memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) &&
                    (memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
                {
                    m_adapter_detail.support_host_visible_vram = true;
                    m_adapter_detail.host_visible_vram_budget = budget_properties.heapBudget[heap_index] ?
                    budget_properties.heapBudget[heap_index] : memory_properties.memoryHeaps[heap_index].size;
                    break;
                }
            }
        }
    }
    else
#endif
    {
        VkPhysicalDeviceMemoryProperties memory_properties;
        vkGetPhysicalDeviceMemoryProperties(m_physical_device, &memory_properties);
        for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
        {
            const uint32_t heap_index = memory_properties.memoryTypes[i].heapIndex;
            if (memory_properties.memoryHeaps[heap_index].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            {
                if ((memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) &&
                    (memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
                {
                    m_adapter_detail.support_host_visible_vram = true;
                    m_adapter_detail.host_visible_vram_budget = memory_properties.memoryHeaps[heap_index].size;
                    break;
                }
            }
        }
    }
}



AMAZING_NAMESPACE_END
