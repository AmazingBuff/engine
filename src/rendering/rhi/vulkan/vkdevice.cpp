//
// Created by AmazingBuff on 2025/5/7.
//

#include "vkdevice.h"
#include "vkadapter.h"
#include "vkinstance.h"
#include "vkqueue.h"
#include "vkfence.h"
#include "internal/vkdescriptor_pool.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKDevice::VKDevice(GPUAdapter const* adapter, GPUDeviceCreateInfo const& info)
    : m_device(nullptr), m_pipeline_cache(nullptr), m_allocator(nullptr), m_device_table{}
{
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(adapter);
    VKInstance const* vk_instance = static_cast<VKInstance const*>(vk_adapter->m_ref_instance);

    // queue
    Vector<VkDeviceQueueCreateInfo> queue_create_infos;
    queue_create_infos.resize(info.queue_groups.size());
    for (size_t i = 0; i < info.queue_groups.size(); i++)
    {
        const GPUQueueGroup& queue_group = info.queue_groups[i];
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueCount = queue_group.queue_count;
        queue_create_infos[i].queueFamilyIndex = vk_adapter->m_queue_family_indices[to_underlying(queue_group.queue_type)];
        queue_create_infos[i].pQueuePriorities = VK_Queue_Priorities;
    }

    VkDeviceCreateInfo device_create_info{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos = queue_create_infos.data(),
        .enabledExtensionCount = static_cast<uint32_t>(vk_adapter->m_device_extensions.size()),
        .ppEnabledExtensionNames = vk_adapter->m_device_extensions.data(),
        .pEnabledFeatures = nullptr,
    };

    VK_CHECK_RESULT(vkCreateDevice(vk_adapter->m_physical_device, &device_create_info, &VK_Allocation_Callbacks, &m_device));

    // for signal device
    volkLoadDeviceTable(&m_device_table, m_device);

    // get queue
    for (GPUQueueGroup const& queue_group : info.queue_groups)
    {
        size_t type = to_underlying(queue_group.queue_type);
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

    // pipeline cache
    if (!info.disable_pipeline_cache)
    {
        VkPipelineCacheCreateInfo pipeline_cache_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = nullptr,
            .initialDataSize = 0,
            .pInitialData = nullptr
        };
        VK_CHECK_RESULT(m_device_table.vkCreatePipelineCache(m_device, &pipeline_cache_create_info, &VK_Allocation_Callbacks, &m_pipeline_cache));
    }

    // vma
    VmaAllocatorCreateInfo allocation_info{
        .flags = VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT,
        .physicalDevice = vk_adapter->m_physical_device,
        .device = m_device,
        .pAllocationCallbacks = &VK_Allocation_Callbacks,
        .instance = vk_instance->m_instance,
        .vulkanApiVersion = VK_API_VERSION_1_0,
    };

    VmaVulkanFunctions vma_functions;
    VK_CHECK_RESULT(vmaImportVulkanFunctionsFromVolk(&allocation_info, &vma_functions));

    allocation_info.pVulkanFunctions = &vma_functions;
    VK_CHECK_RESULT(vmaCreateAllocator(&allocation_info, &m_allocator));

    m_descriptor_pool = PLACEMENT_NEW(VKDescriptorPool, sizeof(VKDescriptorPool), m_device, &m_device_table);

    m_ref_adapter = adapter;
}

VKDevice::~VKDevice()
{
    PLACEMENT_DELETE(VKDescriptorPool, m_descriptor_pool);
    vmaDestroyAllocator(m_allocator);
    if (m_pipeline_cache)
        m_device_table.vkDestroyPipelineCache(m_device, m_pipeline_cache, &VK_Allocation_Callbacks);

    for (uint8_t i = 0; i < GPU_Queue_Type_Count; i++)
    {
        for (size_t j = 0; j < m_command_queues[i].size(); j++)
        {
            PLACEMENT_DELETE(VKQueue, m_command_queues[i][j]);
            m_command_queues[i][j] = nullptr;
        }
    }

    vkDestroyDevice(m_device, &VK_Allocation_Callbacks);
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