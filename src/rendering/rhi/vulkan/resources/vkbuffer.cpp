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
    : m_buffer(nullptr), m_allocation(nullptr)
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(device);
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(vk_device->m_ref_adapter);

    size_t allocation_size = info.size;
    if (FLAG_IDENTITY(info.type, GPUResourceType::e_uniform_buffer))
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
    if (FLAG_IDENTITY(info.flags, GPUBufferFlag::e_dedicated))
        vma_create_info.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    if (FLAG_IDENTITY(info.flags, GPUBufferFlag::e_persistent_map))
        vma_create_info.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
    if ((FLAG_IDENTITY(info.flags, GPUBufferFlag::e_host_visible) && info.usage == GPUMemoryUsage::e_gpu_only) ||
        (FLAG_IDENTITY(info.flags, GPUBufferFlag::e_persistent_map) && info.usage == GPUMemoryUsage::e_gpu_only))
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

    // debug name
    if (!info.name.empty())
        vk_device->set_debug_name(reinterpret_cast<size_t>(m_buffer), VK_OBJECT_TYPE_BUFFER, info.name);

    m_info = Allocator<GPUBufferInfo>::allocate(1);
    m_info->type = info.type;
    m_info->size = info.size;
    m_info->memory_usage = info.usage;
    m_info->mapped_data = allocation_info.pMappedData;
    m_info->flags = info.flags;
    m_info->state = info.state;

    m_ref_device = device;
}

VKBuffer::~VKBuffer()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);

    Allocator<GPUBufferInfo>::deallocate(m_info);
    vmaDestroyBuffer(vk_device->m_allocator, m_buffer, m_allocation);
}

void VKBuffer::map(size_t offset, size_t size, const void* data) const
{
    if (FLAG_IDENTITY(m_info->flags, GPUBufferFlag::e_persistent_map))
        memcpy(static_cast<uint8_t*>(m_info->mapped_data) + offset, data, size);
    else
    {
        VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);

        VK_CHECK_RESULT(vmaMapMemory(vk_device->m_allocator, m_allocation, &m_info->mapped_data));
        m_info->mapped_data = static_cast<uint8_t*>(m_info->mapped_data) + offset;
    }
}

void VKBuffer::unmap() const
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);
    vmaUnmapMemory(vk_device->m_allocator, m_allocation);
    m_info->mapped_data = nullptr;
}

AMAZING_NAMESPACE_END
