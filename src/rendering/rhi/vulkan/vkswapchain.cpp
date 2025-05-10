//
// Created by AmazingBuff on 25-5-10.
//

#include "vkswapchain.h"
#include "vkadapter.h"
#include "vkdevice.h"
#include "vksurface.h"
#include "vkfence.h"
#include "vksemaphore.h"
#include "resources/vktexture.h"
#include "resources/vktexture_view.h"
#include "utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKSwapChain::VKSwapChain(GPUDevice const* device, GPUSwapChainCreateInfo const& info) : m_swap_chain(nullptr)
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(device);
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(vk_device->m_ref_adapter);
    VKSurface const* vk_surface = reinterpret_cast<VKSurface const*>(info.surface);

    VkSurfaceCapabilitiesKHR surface_capabilities;
    VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_adapter->m_physical_device, vk_surface->m_surface, &surface_capabilities));
    uint32_t image_count = info.frame_count;
    if (surface_capabilities.maxImageCount > 0 && image_count > surface_capabilities.maxImageCount)
        image_count = surface_capabilities.maxImageCount;
    else if (image_count < surface_capabilities.minImageCount)
        image_count = surface_capabilities.minImageCount;

    // format
    VkSurfaceFormatKHR surface_format{
        .format = VK_FORMAT_UNDEFINED,
    };
    VkFormat required_format = transfer_format(info.format);
    Vector<VkSurfaceFormatKHR> formats = enumerate_properties(vkGetPhysicalDeviceSurfaceFormatsKHR, vk_adapter->m_physical_device, vk_surface->m_surface);
    for (VkSurfaceFormatKHR const& format : formats)
    {
        if (format.format == required_format)
        {
            surface_format = format;
            break;
        }
    }
    if (surface_format.format == VK_FORMAT_UNDEFINED)
        RENDERING_LOG_ERROR("unsupported surface format for required format {}!", to_underlying(required_format));

    // present mode
    static constexpr VkPresentModeKHR preferred_mode_list[] = {
        VK_PRESENT_MODE_IMMEDIATE_KHR,    // normal
        VK_PRESENT_MODE_MAILBOX_KHR,      // low latency
        VK_PRESENT_MODE_FIFO_RELAXED_KHR, // minimize stuttering
        VK_PRESENT_MODE_FIFO_KHR          // low power consumption
    };
    VkPresentModeKHR present = VK_PRESENT_MODE_FIFO_KHR;
    size_t preferred_mode_start = info.enable_vsync ? 1 : 0;
    Vector<VkPresentModeKHR> present_modes = enumerate_properties(vkGetPhysicalDeviceSurfacePresentModesKHR, vk_adapter->m_physical_device, vk_surface->m_surface);
    for (size_t i = preferred_mode_start; i < array_size(preferred_mode_list); i++)
    {
        bool found = false;
        for (VkPresentModeKHR const& present_mode : present_modes)
        {
            if (present_mode == preferred_mode_list[i])
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            present = preferred_mode_list[i];
            break;
        }
    }

    static constexpr VkCompositeAlphaFlagBitsKHR composite_alpha_flags[] = {
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
    };
    VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;
    for (VkCompositeAlphaFlagBitsKHR const& composite_alpha_flag : composite_alpha_flags)
    {
        if (surface_capabilities.supportedCompositeAlpha & composite_alpha_flag)
        {
            composite_alpha = composite_alpha_flag;
            break;
        }
    }
    RENDERING_ASSERT(composite_alpha != VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR, "failed to find suitable composite alpha!");

    VkExtent2D extent{
        .width = std::clamp(info.width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width),
        .height = std::clamp(info.height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height),
    };

    VkSwapchainCreateInfoKHR swap_chain_create_info{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = vk_surface->m_surface,
        .minImageCount = image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surface_capabilities.currentTransform,
        .compositeAlpha = composite_alpha,
        .presentMode = present,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    VK_CHECK_RESULT(vk_device->m_device_table.vkCreateSwapchainKHR(vk_device->m_device, &swap_chain_create_info, &VK_Allocation_Callbacks, &m_swap_chain));

    Vector<VkImage> swap_chain_images = enumerate_properties(vk_device->m_device_table.vkGetSwapchainImagesKHR, vk_device->m_device, m_swap_chain);

    m_back_textures.resize(swap_chain_images.size());
    for (uint32_t i = 0; i < swap_chain_images.size(); i++)
    {
        VKTexture* back_texture = PLACEMENT_NEW(VKTexture, sizeof(VKTexture));
        // texture info will be freed automatically in ~VKTexture()
        back_texture->m_info = Allocator<VKTexture::GPUTextureInfo>::allocate(1);
        back_texture->m_image = swap_chain_images[i];
        back_texture->m_info->is_cube = false;
        back_texture->m_info->array_layers = 1;
        back_texture->m_info->sample_count = GPUSampleCount::e_1; // TODO: ?
        back_texture->m_info->format = transfer_format_reverse(surface_format.format);
        back_texture->m_info->aspect_mask = transfer_image_aspect(transfer_format(back_texture->m_info->format), false);
        back_texture->m_info->depth = 1;
        back_texture->m_info->width = extent.width;
        back_texture->m_info->height = extent.height;
        back_texture->m_info->mip_levels = 1;
        back_texture->m_info->node_index = GPU_Node_Index;
        back_texture->m_info->owns_image = false;

        GPUTextureViewCreateInfo view_info{
            .texture = back_texture,
            .format = info.format,
            .usage = GPUTextureViewUsageFlag::e_rtv_dsv,
            .aspect = GPUTextureViewAspectFlag::e_color,
            .type = GPUTextureType::e_2d,
            .array_layers = 1,
        };

        VKTextureView* back_texture_view = PLACEMENT_NEW(VKTextureView, sizeof(VKTextureView), device, view_info);

        m_back_textures[i] = { back_texture, back_texture_view };
    }

    m_ref_device = device;
}

VKSwapChain::~VKSwapChain()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);

    vk_device->m_device_table.vkDestroySwapchainKHR(vk_device->m_device, m_swap_chain, &VK_Allocation_Callbacks);
}

uint32_t VKSwapChain::acquire_next_frame(GPUSemaphore const* semaphore, GPUFence const* fence)
{
    VKFence const* vk_fence = static_cast<VKFence const*>(fence);
    VKSemaphore const* vk_semaphore = static_cast<VKSemaphore const*>(semaphore);
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);

    VkSemaphore single_semaphore = vk_semaphore ? vk_semaphore->m_semaphore : nullptr;
    VkFence single_fence = vk_fence ? vk_fence->m_fence : nullptr;

    uint32_t image_index;
    VkResult result = vk_device->m_device_table.vkAcquireNextImageKHR(vk_device->m_device, m_swap_chain,
        std::numeric_limits<uint64_t>::max(), single_semaphore, single_fence, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        return std::numeric_limits<uint32_t>::max();

    VK_CHECK_RESULT(result);

    return image_index;
}

GPUTexture const* VKSwapChain::fetch_back_texture(uint32_t index) const
{

    return nullptr;
}

GPUTextureView const* VKSwapChain::fetch_back_texture_view(uint32_t index) const
{

    return nullptr;
}

AMAZING_NAMESPACE_END