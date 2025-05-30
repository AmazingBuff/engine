//
// Created by AmazingBuff on 2025/5/26.
//

#ifndef VKCOMPUTE_PASS_ENCODER_H
#define VKCOMPUTE_PASS_ENCODER_H

#include "vk.h"
#include "rendering/rhi/common/compute_pass_encoder.h"

AMAZING_NAMESPACE_BEGIN

class VKCommandBuffer;

class VKComputePassEncoder final : public GPUComputePassEncoder
{
public:
    void bind_descriptor_set(GPUDescriptorSet const* set) override;
    void bind_pipeline(GPUComputePipeline const* pipeline) override;
    void set_push_constant(GPURootSignature const* root_signature, String const& name, void const* data) override;
    void dispatch(uint32_t x, uint32_t y, uint32_t z) override;
private:
    VKComputePassEncoder();

private:
    VKCommandBuffer* m_command;

    friend class VKCommandBuffer;
};

AMAZING_NAMESPACE_END

#endif //VKCOMPUTE_PASS_ENCODER_H
