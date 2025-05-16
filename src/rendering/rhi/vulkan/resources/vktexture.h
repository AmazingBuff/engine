//
// Created by AmazingBuff on 2025/5/8.
//

#ifndef VKTEXTURE_H
#define VKTEXTURE_H

#include "rendering/rhi/common/texture.h"
#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

class VKTexture final : public GPUTexture
{
public:
    VKTexture(GPUDevice const* device, GPUTextureCreateInfo const& info);
    ~VKTexture() override;
private:
    VKTexture();
    static VkImageType transfer_image_type(GPUTextureCreateInfo const& info);
    static VkFormatFeatureFlags transfer_image_format_features(VkImageUsageFlags usage);
private:
    VkImage m_image;
    VmaAllocation m_allocation;

    struct VulkanTiledTextureInfo : GPUTextureInfo
    {
        struct VulkanTiledPackingMipMapping
        {
            VmaAllocation allocation;
            uint32_t sparse_tail_stride;
            uint32_t sparse_tail_offset;
            uint32_t sparse_tail_size;
        };

        struct VulkanTiledSubresourceMapping
        {
            VmaAllocation allocation;
            GPUTiledSubresourceInfo subresource_info;
        };

        GPUTiledTextureInfo tiled_info;
        Vector<VulkanTiledSubresourceMapping> subresources;
        Vector<VulkanTiledPackingMipMapping> packings;
        bool single_tail;
    };

    friend class VKSwapChain;
    friend class VKTextureView;
    friend class VKCommandBuffer;
};

AMAZING_NAMESPACE_END

#endif //VKTEXTURE_H
