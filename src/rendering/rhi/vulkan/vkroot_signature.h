//
// Created by AmazingBuff on 2025/5/12.
//

#ifndef VKROOT_SIGNATURE_H
#define VKROOT_SIGNATURE_H

#include "vk.h"
#include "rendering/rhi/common/root_signature.h"

AMAZING_NAMESPACE_BEGIN

class VKRootSignature final : public GPURootSignature
{
public:
    VKRootSignature(GPUDevice const* device, GPURootSignatureCreateInfo const& info);
    ~VKRootSignature() override;
private:
    VkPipelineLayout m_pipeline_layout;

    struct VulkanDescriptorLayout
    {
        VkDescriptorSetLayout set_layout;
        VkDescriptorSet set;
        VkDescriptorUpdateTemplate update_template;
        uint32_t update_entry_count;
    };

    // set <---> layout
    Map<uint32_t, VulkanDescriptorLayout> m_descriptor_layouts;
    // sampler descriptors is a subset of descriptor layouts because layout must be ordered
    HashMap<uint32_t, VulkanDescriptorLayout> m_static_sampler_descriptors;
    VkPushConstantRange* m_push_constant_ranges;

    friend class VKDescriptorSet;
    friend class VKGraphicsPipeline;
    friend class VKGraphicsPassEncoder;
};

AMAZING_NAMESPACE_END

#endif //VKROOT_SIGNATURE_H
