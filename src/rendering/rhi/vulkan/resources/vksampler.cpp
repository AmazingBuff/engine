//
// Created by AmazingBuff on 2025/5/9.
//

#include "vksampler.h"
#include "rendering/rhi/vulkan/vkdevice.h"
#include "rendering/rhi/vulkan/vkadapter.h"
#include "rendering/rhi/vulkan/utils/vk_macro.h"
#include "rendering/rhi/vulkan/utils/vk_utils.h"

AMAZING_NAMESPACE_BEGIN

VKSampler::VKSampler(GPUDevice const* device, GPUSamplerCreateInfo const& info) : m_sampler(nullptr)
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(device);

    VkSamplerCreateInfo sampler_info{
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = transfer_filter(info.mag_filter),
        .minFilter = transfer_filter(info.min_filter),
        .mipmapMode = transfer_mipmap_mode(info.mipmap_mode),
        .addressModeU = transfer_address_mode(info.address_u),
        .addressModeV = transfer_address_mode(info.address_v),
        .addressModeW = transfer_address_mode(info.address_w),
        .mipLodBias = info.mip_lod_bias,
        .anisotropyEnable = info.max_anisotropy > 0.0f ? VK_TRUE : VK_FALSE,
        .maxAnisotropy = info.max_anisotropy,
        .compareEnable = info.compare_mode != GPUCompareMode::e_never ? VK_TRUE : VK_FALSE,
        .compareOp = Compare_Mode_Map[to_underlying(info.compare_mode)],
        .minLod = 0.0f,
        .maxLod = info.mipmap_mode == GPUMipMapMode::e_linear ? std::numeric_limits<float>::max() : 0.0f,
        .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };
    VK_CHECK_RESULT(vk_device->m_device_table.vkCreateSampler(vk_device->m_device, &sampler_info, VK_Allocation_Callbacks_Ptr, &m_sampler));

    m_ref_device = device;
}

VKSampler::~VKSampler()
{
    VKDevice const* vk_device = static_cast<VKDevice const*>(m_ref_device);
    vk_device->m_device_table.vkDestroySampler(vk_device->m_device, m_sampler, VK_Allocation_Callbacks_Ptr);
}

AMAZING_NAMESPACE_END
