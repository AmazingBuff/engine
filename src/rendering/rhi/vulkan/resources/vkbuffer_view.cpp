//
// Created by AmazingBuff on 2025/7/7.
//

#include "vkbuffer_view.h"
#include "vkbuffer.h"
#include "rendering/rhi/vulkan/vkdevice.h"
#include "rendering/rhi/vulkan/vkadapter.h"
#include "rendering/rhi/vulkan/utils/vk_macro.h"
#include "rendering/rhi/vulkan/utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKBufferView::VKBufferView(GPUBufferViewCreateInfo const& info)
{
    VKBuffer const* buffer = static_cast<VKBuffer const*>(info.buffer);
    VKDevice const* vk_device = static_cast<VKDevice const*>(buffer->m_ref_device);
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(vk_device->m_ref_adapter);

    if (FLAG_IDENTITY(info.usage, GPUBufferViewUsage::e_srv) || FLAG_IDENTITY(info.usage, GPUBufferViewUsage::e_uav))
    {
        VkFormat format = transfer_format(info.format);
        VkBufferViewCreateInfo buffer_view_info{
            .sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .buffer = buffer->m_buffer,
            .format = format,
            .offset = info.data_array.first_element * info.data_array.element_stride,
            .range = info.data_array.element_count * info.data_array.element_stride,
        };

        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(vk_adapter->m_physical_device, format, &format_properties);

        if (FLAG_IDENTITY(info.usage, GPUBufferViewUsage::e_srv))
        {
            if (format_properties.bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT)
                VK_CHECK_RESULT(vk_device->m_device_table.vkCreateBufferView(vk_device->m_device, &buffer_view_info, VK_Allocation_Callbacks_Ptr, &m_srv_view))
            else
                RENDERING_LOG_WARNING("unable to create uniform texel buffer view for format {}!", to_underlying(info.format));
        }

        if (FLAG_IDENTITY(info.usage, GPUBufferViewUsage::e_uav))
        {
            if (format_properties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT)
                VK_CHECK_RESULT(vk_device->m_device_table.vkCreateBufferView(vk_device->m_device, &buffer_view_info, VK_Allocation_Callbacks_Ptr, &m_uav_view))
            else
                RENDERING_LOG_WARNING("unable to create uniform storage buffer view for format {}!", to_underlying(info.format));
        }
    }

    m_usage = info.usage;
    m_format = info.format;
    m_ref_buffer = buffer;
}

VKBufferView::~VKBufferView()
{
    VKBuffer const* buffer = static_cast<VKBuffer const*>(m_ref_buffer);
    VKDevice const* vk_device = static_cast<VKDevice const*>(buffer->m_ref_device);

    if (m_srv_view)
        vk_device->m_device_table.vkDestroyBufferView(vk_device->m_device, m_srv_view, VK_Allocation_Callbacks_Ptr);
    if (m_uav_view)
        vk_device->m_device_table.vkDestroyBufferView(vk_device->m_device, m_uav_view, VK_Allocation_Callbacks_Ptr);
}

AMAZING_NAMESPACE_END
