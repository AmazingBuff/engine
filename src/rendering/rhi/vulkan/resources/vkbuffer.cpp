//
// Created by AmazingBuff on 2025/5/8.
//

#include "vkbuffer.h"
#include "rendering/rhi/vulkan/vkdevice.h"
#include "rendering/rhi/vulkan/vkadapter.h"
#include "rendering/rhi/vulkan/utils/vk_macro.h"
#include "rendering/rhi/vulkan/utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKBuffer::VKBuffer(GPUDevice const* device, GPUBufferCreateInfo const& info)
    : m_buffer(nullptr), m_storage_texel_view(nullptr), m_uniform_texel_view(nullptr), m_allocation(nullptr)
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(device);
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(vk_device->m_ref_adapter);

    size_t allocation_size = info.size;
    if (info.type & GPUResourceTypeFlag::e_uniform_buffer)
    {
        size_t alignment = vk_adapter->m_adapter_detail.uniform_buffer_alignment;
        allocation_size = align_to(allocation_size, alignment);
    }

    VkBufferCreateInfo buffer_create_info{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .flags = 0,
        .size = allocation_size,
        .usage = transfer_buffer_usage(info.type, info.format != GPUFormat::e_undefined),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    if (info.usage == GPUMemoryUsage::e_gpu_only || info.usage == GPUMemoryUsage::e_gpu_to_cpu)
        buffer_create_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    // vma
    VmaAllocationCreateInfo vma_create_info{
        .usage = static_cast<VmaMemoryUsage>(info.usage),
    };
    if (info.flags & GPUBufferFlagsFlag::e_dedicated)
        vma_create_info.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    if (info.flags & GPUBufferFlagsFlag::e_persistent_map)
        vma_create_info.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
    if ((info.flags & GPUBufferFlagsFlag::e_host_visible && info.usage == GPUMemoryUsage::e_gpu_only) ||
        (info.flags & GPUBufferFlagsFlag::e_persistent_map && info.usage == GPUMemoryUsage::e_gpu_only))
        vma_create_info.flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    if (info.usage == GPUMemoryUsage::e_cpu_to_gpu)
    {
        vma_create_info.usage = VMA_MEMORY_USAGE_AUTO;
        vma_create_info.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    }
    if (info.usage == GPUMemoryUsage::e_gpu_to_cpu)
    {
        vma_create_info.usage = VMA_MEMORY_USAGE_AUTO;
        vma_create_info.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
    }

    VmaAllocationInfo allocation_info;
    VK_CHECK_RESULT(vmaCreateBuffer(vk_device->m_allocator, &buffer_create_info, &vma_create_info, &m_buffer, &m_allocation, &allocation_info));

    // buffer view
    if (buffer_create_info.usage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT || buffer_create_info.usage & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT)
    {
        const VkFormat format = transfer_format(info.format);
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(vk_adapter->m_physical_device, format, &format_properties);
        VkBufferViewCreateInfo buffer_view_create_info{
            .sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
            .buffer = m_buffer,
            .format = format,
            .offset = info.data_array.first_element * info.data_array.element_stride,
            .range = info.data_array.element_count * info.data_array.element_stride,
        };
        if (buffer_create_info.usage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT)
        {
            if (format_properties.bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT)
                VK_CHECK_RESULT(vk_device->m_device_table.vkCreateBufferView(vk_device->m_device, &buffer_view_create_info, VK_Allocation_Callbacks_Ptr, &m_uniform_texel_view))
            else
                RENDERING_LOG_WARNING("unable to create uniform texel buffer view for format {}!", to_underlying(info.format));
        }
        if (buffer_create_info.usage & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT)
        {
            if (format_properties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT)
                VK_CHECK_RESULT(vk_device->m_device_table.vkCreateBufferView(vk_device->m_device, &buffer_view_create_info, VK_Allocation_Callbacks_Ptr, &m_storage_texel_view))
            else
                RENDERING_LOG_WARNING("unable to create uniform storage buffer view for format {}!", to_underlying(info.format));
        }
    }

    // debug name
    if (!info.name.empty())
        vk_device->set_debug_name(reinterpret_cast<size_t>(m_buffer), VK_OBJECT_TYPE_BUFFER, info.name);

    m_info = Allocator<GPUBufferInfo>::allocate(1);
    m_info->type = info.type;
    m_info->size = info.size;
    m_info->memory_usage = info.usage;
    m_info->mapped_data = allocation_info.pMappedData;
    m_info->flags = info.flags;

    m_ref_device = device;
}

VKBuffer::~VKBuffer()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);

    if (m_uniform_texel_view)
    {
        vk_device->m_device_table.vkDestroyBufferView(vk_device->m_device, m_uniform_texel_view, VK_Allocation_Callbacks_Ptr);
        m_uniform_texel_view = VK_NULL_HANDLE;
    }
    if (m_storage_texel_view)
    {
        vk_device->m_device_table.vkDestroyBufferView(vk_device->m_device, m_storage_texel_view, VK_Allocation_Callbacks_Ptr);
        m_storage_texel_view = VK_NULL_HANDLE;
    }
    vmaDestroyBuffer(vk_device->m_allocator, m_buffer, m_allocation);
}

void VKBuffer::map(size_t offset, size_t size, const void* data)
{
    if (m_info->flags & GPUBufferFlagsFlag::e_persistent_map)
        memcpy(static_cast<uint8_t*>(m_info->mapped_data) + offset, data, size);
    else
    {
        VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);

        VK_CHECK_RESULT(vmaMapMemory(vk_device->m_allocator, m_allocation, &m_info->mapped_data));
        m_info->mapped_data = static_cast<uint8_t*>(m_info->mapped_data) + offset;
    }
}

void VKBuffer::unmap()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);
    vmaUnmapMemory(vk_device->m_allocator, m_allocation);
    m_info->mapped_data = nullptr;
}

AMAZING_NAMESPACE_END
