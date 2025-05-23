//
// Created by AmazingBuff on 2025/5/7.
//

#include "vkdevice.h"
#include "vkadapter.h"
#include "vkinstance.h"
#include "vkqueue.h"
#include "vkcommand_pool.h"
#include "vkcommand_buffer.h"
#include "vkfence.h"
#include "internal/vkdescriptor_pool.h"
#include "internal/vkpass_table.h"
#include "utils/vk_macro.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKDevice::VKDevice(GPUAdapter const* adapter, GPUDeviceCreateInfo const& info)
    : m_device(nullptr), m_pipeline_cache(nullptr), m_allocator(nullptr), m_device_table{}
{
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(adapter);
    VKInstance const* vk_instance = static_cast<VKInstance const*>(vk_adapter->m_ref_instance);

    // queue
    Vector<VkDeviceQueueCreateInfo> queue_create_infos(info.queue_groups.size());
    uint32_t queue_index = 0;
    for (GPUQueueGroup const& queue_group : info.queue_groups)
    {
        uint32_t queue_family_index = vk_adapter->m_queue_family_indices[to_underlying(queue_group.queue_type)];
        if (queue_family_index != std::numeric_limits<uint32_t>::max())
        {
            queue_create_infos[queue_index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[queue_index].queueCount = queue_group.queue_count;
            queue_create_infos[queue_index].queueFamilyIndex = queue_family_index;
            queue_create_infos[queue_index].pQueuePriorities = VK_Queue_Priorities;
            queue_index++;
        }
        else
            RENDERING_LOG_ERROR("not satisfied queue! queue type is {}", to_underlying(queue_group.queue_type));
    }

    VkDeviceCreateInfo device_create_info{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = queue_index,
        .pQueueCreateInfos = queue_create_infos.data(),
        .enabledLayerCount = static_cast<uint32_t>(vk_adapter->m_device_layers.size()),
        .ppEnabledLayerNames = vk_adapter->m_device_layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(vk_adapter->m_device_extensions.size()),
        .ppEnabledExtensionNames = vk_adapter->m_device_extensions.data(),
        .pEnabledFeatures = &vk_adapter->m_vulkan_detail.device_features,
    };

    VK_CHECK_RESULT(vkCreateDevice(vk_adapter->m_physical_device, &device_create_info, VK_Allocation_Callbacks_Ptr, &m_device));
    m_ref_adapter = adapter;

    // for signal device
    volkLoadDeviceTable(&m_device_table, m_device);

    // get queue
    for (GPUQueueGroup const& queue_group : info.queue_groups)
    {
        size_t type = to_underlying(queue_group.queue_type);
        uint32_t queue_family_index = vk_adapter->m_queue_family_indices[type];
        if (queue_family_index != std::numeric_limits<uint32_t>::max())
        {
            m_command_queues[type].resize(queue_group.queue_count);
            for (uint32_t j = 0; j < queue_group.queue_count; j++)
            {
                VKQueue* queue = PLACEMENT_NEW(VKQueue, sizeof(VKQueue));
                queue->m_type = queue_group.queue_type;

                m_device_table.vkGetDeviceQueue(m_device, vk_adapter->m_queue_family_indices[type], j, &queue->m_queue);

                queue->m_ref_device = this;
                m_command_queues[type][j] = queue;
            }
        }
    }

    // pipeline cache
    if (!info.disable_pipeline_cache)
    {
        VkPipelineCacheCreateInfo pipeline_cache_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = nullptr,
            .initialDataSize = 0,
            .pInitialData = nullptr
        };
        VK_CHECK_RESULT(m_device_table.vkCreatePipelineCache(m_device, &pipeline_cache_create_info, VK_Allocation_Callbacks_Ptr, &m_pipeline_cache));
    }

    // vma
    VmaAllocatorCreateInfo allocation_info{
        .flags = VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT,
        .physicalDevice = vk_adapter->m_physical_device,
        .device = m_device,
        .pAllocationCallbacks = VK_Allocation_Callbacks_Ptr,
        .instance = vk_instance->m_instance,
        .vulkanApiVersion = VK_API_VERSION_1_1,
    };

    VmaVulkanFunctions vma_functions;
    VK_CHECK_RESULT(vmaImportVulkanFunctionsFromVolk(&allocation_info, &vma_functions));

    allocation_info.pVulkanFunctions = &vma_functions;
    VK_CHECK_RESULT(vmaCreateAllocator(&allocation_info, &m_allocator));

    m_descriptor_pool = PLACEMENT_NEW(VKDescriptorPool, sizeof(VKDescriptorPool), this);
    m_pass_table = PLACEMENT_NEW(VKPassTable, sizeof(VKPassTable));

    VKQueue* internal_queue = nullptr;
    if (m_command_queues[to_underlying(GPUQueueType::e_transfer)].empty())
        internal_queue = m_command_queues[to_underlying(GPUQueueType::e_graphics)][0];
    else
        internal_queue = m_command_queues[to_underlying(GPUQueueType::e_transfer)][0];

    GPUCommandPoolCreateInfo command_pool_create_info{.name = "internal command pool"};
    m_internal_command_pool = PLACEMENT_NEW(VKCommandPool, sizeof(VKCommandPool), internal_queue, command_pool_create_info);
    GPUCommandBufferCreateInfo command_buffer_create_info{};
    m_internal_command_buffer = PLACEMENT_NEW(VKCommandBuffer, sizeof(VKCommandBuffer), m_internal_command_pool, command_buffer_create_info);
}

VKDevice::~VKDevice()
{
    PLACEMENT_DELETE(VKCommandBuffer, m_internal_command_buffer);
    PLACEMENT_DELETE(VKCommandPool, m_internal_command_pool);
    PLACEMENT_DELETE(VKPassTable, m_pass_table);
    PLACEMENT_DELETE(VKDescriptorPool, m_descriptor_pool);
    vmaDestroyAllocator(m_allocator);
    if (m_pipeline_cache)
        m_device_table.vkDestroyPipelineCache(m_device, m_pipeline_cache, VK_Allocation_Callbacks_Ptr);

    for (uint8_t i = 0; i < GPU_Queue_Type_Count; i++)
    {
        for (size_t j = 0; j < m_command_queues[i].size(); j++)
        {
            PLACEMENT_DELETE(VKQueue, m_command_queues[i][j]);
            m_command_queues[i][j] = nullptr;
        }
    }

    m_device_table.vkDestroyDevice(m_device, VK_Allocation_Callbacks_Ptr);
}

GPUQueue const* VKDevice::fetch_queue(GPUQueueType type, uint32_t index) const
{
    return m_command_queues[to_underlying(type)][index];
}


void VKDevice::set_debug_name(size_t handle, VkObjectType type, const String& name) const
{
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(m_ref_adapter);
    VKInstance const* vk_instance = static_cast<VKInstance const*>(vk_adapter->m_ref_instance);

    if (vk_instance->m_debug_messenger)
    {
        VkDebugUtilsObjectNameInfoEXT name_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .objectType = type,
            .objectHandle = handle,
            .pObjectName = name.c_str()
        };
        VK_CHECK_RESULT(vkSetDebugUtilsObjectNameEXT(m_device, &name_info));
    }
}

AMAZING_NAMESPACE_END