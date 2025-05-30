//
// Created by AmazingBuff on 2025/5/12.
//

#ifndef VKDESCRIPTOR_SET_H
#define VKDESCRIPTOR_SET_H

#include "vk.h"
#include "rendering/rhi/common/descriptor_set.h"

AMAZING_NAMESPACE_BEGIN

class VKDescriptorSet final : public GPUDescriptorSet
{
public:
    explicit VKDescriptorSet(GPUDescriptorSetCreateInfo const& info);
    ~VKDescriptorSet() override;

    void update(GPUDescriptorData const* descriptor_data, uint32_t descriptor_data_count) override;
private:
    VkDescriptorSet m_descriptor_set;
    VulkanDescriptorUpdateData* m_update_data;

    friend class VKGraphicsPassEncoder;
    friend class VKComputePassEncoder;
};

AMAZING_NAMESPACE_END

#endif //VKDESCRIPTOR_SET_H
