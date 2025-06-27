//
// Created by AmazingBuff on 2025/5/8.
//

#include "vktexture_view.h"
#include "vktexture.h"
#include "rendering/rhi/vulkan/vkdevice.h"
#include "rendering/rhi/vulkan/vkadapter.h"
#include "rendering/rhi/vulkan/utils/vk_macro.h"
#include "rendering/rhi/vulkan/utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKTextureView::VKTextureView(GPUTextureViewCreateInfo const& info) : m_rtv_dsv_view(nullptr), m_srv_view(nullptr), m_uav_view(nullptr)
{
    VKTexture const* texture = static_cast<VKTexture const*>(info.texture);
    VKDevice const* vk_device = static_cast<VKDevice const*>(texture->m_ref_device);
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(vk_device->m_ref_adapter);

    VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    VkImageType image_type = texture->m_info->depth > 1 ? VK_IMAGE_TYPE_3D : texture->m_info->height > 1 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_1D;
    switch (image_type)
    {
    case VK_IMAGE_TYPE_1D:
        view_type = info.array_layers > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
        break;
    case VK_IMAGE_TYPE_2D:
        if (texture->m_info->is_cube)
            view_type = info.type == GPUTextureType::e_cube_array ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
        else
            view_type = (info.type == GPUTextureType::e_2d_array || info.type == GPUTextureType::e_2dms_array) ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
        break;
    case VK_IMAGE_TYPE_3D:
        RENDERING_ASSERT(info.array_layers == 1, "unsupported array layers for 3d texture array! In vulkan, it must be equal to 1.");
        view_type = VK_IMAGE_VIEW_TYPE_3D;
        break;
    default:
        RENDERING_LOG_ERROR("unrecognized image view type!");
        break;
    }

    VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_NONE;
    if (FLAG_IDENTITY(info.aspect, GPUTextureViewAspect::e_color))
        aspect_flags |= VK_IMAGE_ASPECT_COLOR_BIT;
    if (FLAG_IDENTITY(info.aspect, GPUTextureViewAspect::e_depth))
        aspect_flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
    if (FLAG_IDENTITY(info.aspect, GPUTextureViewAspect::e_stencil))
        aspect_flags |= VK_IMAGE_ASPECT_STENCIL_BIT;

    VkImageViewCreateInfo view_create_info{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = texture->m_image,
        .viewType = view_type,
        .format = transfer_format(info.format),
        .components{
            .r = VK_COMPONENT_SWIZZLE_R,
            .g = VK_COMPONENT_SWIZZLE_G,
            .b = VK_COMPONENT_SWIZZLE_B,
            .a = VK_COMPONENT_SWIZZLE_A,
        },
        .subresourceRange{
            .aspectMask = aspect_flags,
            .baseMipLevel = info.base_mip_level,
            .levelCount = info.mip_levels,
            .baseArrayLayer = info.base_array_layer,
            .layerCount = info.array_layers,
        }
    };
    // srv
    if (FLAG_IDENTITY(info.usage, GPUTextureViewUsage::e_srv))
        VK_CHECK_RESULT(vk_device->m_device_table.vkCreateImageView(vk_device->m_device, &view_create_info, VK_Allocation_Callbacks_Ptr, &m_srv_view));

    // rtv & dsv
    if (FLAG_IDENTITY(info.usage, GPUTextureViewUsage::e_rtv_dsv))
        VK_CHECK_RESULT(vk_device->m_device_table.vkCreateImageView(vk_device->m_device, &view_create_info, VK_Allocation_Callbacks_Ptr, &m_rtv_dsv_view));

    if (FLAG_IDENTITY(info.usage, GPUTextureViewUsage::e_uav))
    {
        // all cube map will be used as image 2d array for image load / store ops
        if (view_create_info.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY || view_create_info.viewType == VK_IMAGE_VIEW_TYPE_CUBE)
            view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        VK_CHECK_RESULT(vk_device->m_device_table.vkCreateImageView(vk_device->m_device, &view_create_info, VK_Allocation_Callbacks_Ptr, &m_uav_view));
    }

    m_ref_texture = texture;
}

VKTextureView::~VKTextureView()
{
    VKTexture const* texture = static_cast<VKTexture const*>(m_ref_texture);
    VKDevice const* vk_device = static_cast<VKDevice const*>(texture->m_ref_device);

    if (m_rtv_dsv_view)
        vk_device->m_device_table.vkDestroyImageView(vk_device->m_device, m_rtv_dsv_view, VK_Allocation_Callbacks_Ptr);
    if (m_srv_view)
        vk_device->m_device_table.vkDestroyImageView(vk_device->m_device, m_srv_view, VK_Allocation_Callbacks_Ptr);
    if (m_uav_view)
        vk_device->m_device_table.vkDestroyImageView(vk_device->m_device, m_uav_view, VK_Allocation_Callbacks_Ptr);
}


AMAZING_NAMESPACE_END