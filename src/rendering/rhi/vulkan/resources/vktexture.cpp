//
// Created by AmazingBuff on 2025/5/8.
//

#include "vktexture.h"
#include "rendering/rhi/vulkan/vkdevice.h"
#include "rendering/rhi/vulkan/vkadapter.h"
#include "rendering/rhi/vulkan/utils/vk_macro.h"
#include "rendering/rhi/vulkan/utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VkImageType VKTexture::transfer_image_type(GPUTextureCreateInfo const& info)
{
    if (info.flags & GPUTextureFlagsFlag::e_force_2d)
        return VK_IMAGE_TYPE_2D;
    else if (info.flags & GPUTextureFlagsFlag::e_force_3d)
        return VK_IMAGE_TYPE_3D;
    else
    {
        if (info.depth > 1)
            return VK_IMAGE_TYPE_3D;
        else if (info.height > 1)
            return VK_IMAGE_TYPE_2D;
        else
            return VK_IMAGE_TYPE_1D;
    }
}

VkFormatFeatureFlags VKTexture::transfer_image_format_features(VkImageUsageFlags usage)
{
    if ((usage & VK_IMAGE_USAGE_SAMPLED_BIT) == VK_IMAGE_USAGE_SAMPLED_BIT)
        return VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    else if ((usage & VK_IMAGE_USAGE_STORAGE_BIT) == VK_IMAGE_USAGE_STORAGE_BIT)
        return VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    else if ((usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        return VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    else if ((usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        return VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    else
        return 0;
}


VKTexture::VKTexture() : m_image(nullptr), m_allocation(nullptr) {}

VKTexture::VKTexture(GPUDevice const* device, GPUTextureCreateInfo const& info) : m_image(nullptr), m_allocation(nullptr)
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(device);
    VKAdapter const* vk_adapter = static_cast<VKAdapter const*>(vk_device->m_ref_adapter);

    const GPUAdapter::GPUFormatSupport& format_support = vk_adapter->m_adapter_detail.format_support[to_underlying(info.format)];
    RENDERING_ASSERT(format_support.shader_read, "gpu can't read from this format {}!", to_underlying(info.format));

    VkImageCreateInfo image_create_info{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = transfer_image_type(info),
        .format = transfer_format(info.format),
        .extent{
            .width = info.width,
            .height = info.height,
            .depth = info.depth,
        },
        .mipLevels = info.mip_levels,
        .arrayLayers = info.array_layers,
        .samples = transfer_sample_count(info.sample_count),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = transfer_image_usage(info.type),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    // usage
    if (info.type & GPUResourceTypeFlag::e_render_target)
        image_create_info.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    else if (is_depth_stencil_format(info.format))
        image_create_info.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if ((info.type & GPUResourceTypeFlag::e_texture_cube) == GPUResourceTypeFlag::e_texture_cube)
        image_create_info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    if (image_create_info.imageType == VK_IMAGE_TYPE_3D)
        image_create_info.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT_KHR;

    if (image_create_info.usage & VK_IMAGE_USAGE_SAMPLED_BIT || image_create_info.usage & VK_IMAGE_USAGE_STORAGE_BIT)
        image_create_info.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if (info.flags & GPUTextureFlagsFlag::e_tiled_resource)
    {
        image_create_info.flags |= VK_IMAGE_CREATE_SPARSE_BINDING_BIT;
        image_create_info.flags |= VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT;
    }

    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(vk_adapter->m_physical_device, image_create_info.format, &format_properties);
    VkFormatFeatureFlags feature_flags = transfer_image_format_features(image_create_info.usage);
    RENDERING_ASSERT(format_properties.optimalTilingFeatures & feature_flags, "unsupported format {} for optimal tiling!", to_underlying(image_create_info.format));

    bool is_allocation_dedicated = false;
    bool can_alias_alloc = false;
    if (info.flags & GPUTextureFlagsFlag::e_aliasing_resource || info.flags & GPUTextureFlagsFlag::e_tiled_resource)
        VK_CHECK_RESULT(vk_device->m_device_table.vkCreateImage(vk_device->m_device, &image_create_info, VK_Allocation_Callbacks_Ptr, &m_image))
    else
    {
        VmaAllocationCreateInfo allocation_create_info{
            .usage = VMA_MEMORY_USAGE_GPU_ONLY,
        };
        if (info.flags & GPUTextureFlagsFlag::e_dedicated)
            allocation_create_info.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        allocation_create_info.flags |= VMA_ALLOCATION_CREATE_CAN_ALIAS_BIT;

        VmaAllocationInfo allocation_info;
        VK_CHECK_RESULT(vmaCreateImage(vk_device->m_allocator, &image_create_info, &allocation_create_info, &m_image, &m_allocation, &allocation_info));

        is_allocation_dedicated = allocation_create_info.flags & VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        can_alias_alloc = allocation_create_info.flags & VMA_ALLOCATION_CREATE_CAN_ALIAS_BIT;
    }

    // fill tile info
    if (info.flags & GPUTextureFlagsFlag::e_tiled_resource)
    {
        // todo: may have some problems
        VkMemoryRequirements sparse_memory_requirements;
        vk_device->m_device_table.vkGetImageMemoryRequirements(vk_device->m_device, m_image, &sparse_memory_requirements);
        if (sparse_memory_requirements.size > vk_adapter->m_vulkan_detail.device_properties.limits.sparseAddressSpaceSize)
            RENDERING_LOG_ERROR("requested sparse image size exceeds supported sparse address space size!");

        Vector<VkSparseImageMemoryRequirements> sparse_image_memory_requirements = enumerate_properties(vk_device->m_device_table.vkGetImageSparseMemoryRequirements, vk_device->m_device, m_image);
        if (sparse_image_memory_requirements.empty())
            RENDERING_LOG_ERROR("requested sparse image memory requirements is empty!");

        bool found = false;
        VkSparseImageMemoryRequirements memory_requirements;
        for (VkSparseImageMemoryRequirements const& sparse_image_memory_requirement : sparse_image_memory_requirements)
        {
            if (sparse_image_memory_requirement.formatProperties.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT &&
                !(sparse_image_memory_requirement.formatProperties.flags & VK_SPARSE_IMAGE_FORMAT_NONSTANDARD_BLOCK_SIZE_BIT))
            {
                memory_requirements = sparse_image_memory_requirement;
                found = true;
                break;
            }
        }
        if (!found)
            RENDERING_LOG_ERROR("color aspect sparse image memory requirements not found!");

        VulkanTiledTextureInfo* tiled = Allocator<VulkanTiledTextureInfo>::allocate(1);
        uint32_t subresource_count = info.mip_levels;
        uint32_t total_tiles_count = 0;

        tiled->subresources.resize(subresource_count);
        for (uint32_t i = 0; i < subresource_count; ++i)
        {
            GPUTiledSubresourceInfo& subresource = tiled->subresources[i].subresource_info;

            uint32_t width = std::max(1u, info.width >> i);
            uint32_t height = std::max(1u, info.height >> i);
            uint32_t depth = std::max(1u, info.depth >> i);

            subresource.layers = 0;
            subresource.mip_levels = i;
            subresource.width_in_tiles = division_up(width, memory_requirements.formatProperties.imageGranularity.width);
            subresource.height_in_tiles = division_up(height, memory_requirements.formatProperties.imageGranularity.height);
            subresource.depth_in_tiles = division_up(depth, memory_requirements.formatProperties.imageGranularity.depth);

            total_tiles_count += subresource.width_in_tiles * subresource.height_in_tiles * subresource.depth_in_tiles;
        }

        RENDERING_ASSERT(sparse_memory_requirements.size == total_tiles_count * VK_Sparse_Page_Standard_Size, "unexpected sparse image memory requirements size!");

        tiled->tiled_info.tile_size = VK_Sparse_Page_Standard_Size;
        tiled->tiled_info.tile_count = total_tiles_count;
        tiled->tiled_info.tile_width_in_texels = memory_requirements.formatProperties.imageGranularity.width;
        tiled->tiled_info.tile_height_in_texels = memory_requirements.formatProperties.imageGranularity.height;
        tiled->tiled_info.tile_depth_in_texels = memory_requirements.formatProperties.imageGranularity.depth;
        tiled->tiled_info.packed_mip_start = memory_requirements.imageMipTailFirstLod;
        tiled->tiled_info.packed_mip_count = info.mip_levels - memory_requirements.imageMipTailFirstLod;

        if (memory_requirements.formatProperties.flags & VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT)
        {
            tiled->packings.resize(1);
            tiled->single_tail = true;
        }
        else
        {
            tiled->packings.resize(info.array_layers);
            tiled->single_tail = false;
        }

        for (VulkanTiledTextureInfo::VulkanTiledPackingMipMapping& mip : tiled->packings)
        {
            mip.sparse_tail_stride = memory_requirements.imageMipTailStride;
            mip.sparse_tail_offset = memory_requirements.imageMipTailOffset;
            mip.sparse_tail_size = memory_requirements.imageMipTailSize;
        }

        tiled->is_tiled = 1;
        m_info = tiled;
    }
    else
        m_info = Allocator<GPUTextureInfo>::allocate(1);

    if (!info.name.empty())
        vk_device->set_debug_name(reinterpret_cast<size_t>(m_image), VK_OBJECT_TYPE_IMAGE, info.name);

    // fill
    m_info->aspect_mask = transfer_image_aspect(image_create_info.format, true);
    m_info->owns_image = info.flags & GPUTextureFlagsFlag::e_aliasing_resource ? 0 : 1;
    m_info->is_allocation_dedicated = is_allocation_dedicated;
    m_info->is_aliasing = info.flags & GPUTextureFlagsFlag::e_aliasing_resource ? 1 : 0;
    m_info->can_alias = can_alias_alloc || m_info->is_aliasing;
    m_info->sample_count = info.sample_count;
    m_info->width = info.width;
    m_info->height = info.height;
    m_info->depth = info.depth;
    m_info->mip_levels = info.mip_levels;
    m_info->array_layers = info.array_layers;
    m_info->is_cube = (info.type & GPUResourceTypeFlag::e_texture_cube) == GPUResourceTypeFlag::e_texture_cube;
    m_info->format = info.format;

    m_ref_device = device;
}

VKTexture::~VKTexture()
{
    VKDevice const* device = reinterpret_cast<VKDevice const*>(m_ref_device);
    if (m_info->is_tiled)
    {
        device->m_device_table.vkDestroyImage(device->m_device, m_image, VK_Allocation_Callbacks_Ptr);
        Allocator<VulkanTiledTextureInfo>::deallocate(static_cast<VulkanTiledTextureInfo*>(m_info));
    }
    else
    {
        if (m_allocation)
            vmaDestroyImage(device->m_allocator, m_image, m_allocation);
        Allocator<GPUTextureInfo>::deallocate(m_info);
    }
}




AMAZING_NAMESPACE_END
