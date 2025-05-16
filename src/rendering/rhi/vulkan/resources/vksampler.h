//
// Created by AmazingBuff on 2025/5/9.
//

#ifndef VKSAMPLER_H
#define VKSAMPLER_H

#include "rendering/rhi/common/sampler.h"
#include "rendering/rhi/vulkan/vk.h"

AMAZING_NAMESPACE_BEGIN

class VKSampler final : public GPUSampler
{
public:
    VKSampler(GPUDevice const* device, GPUSamplerCreateInfo const& info);
    ~VKSampler() override;
private:
    VkSampler m_sampler;

    friend class VKRootSignature;
    friend class VKDescriptorSet;
};

AMAZING_NAMESPACE_END

#endif //VKSAMPLER_H
