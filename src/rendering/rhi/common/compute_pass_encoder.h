//
// Created by AmazingBuff on 2025/5/26.
//

#ifndef COMPUTE_PASS_ENCODER_H
#define COMPUTE_PASS_ENCODER_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUComputePassEncoder
{
public:
    GPUComputePassEncoder() = default;
    virtual ~GPUComputePassEncoder() = default;

    virtual void bind_descriptor_set(GPUDescriptorSet const* set) = 0;
    virtual void bind_pipeline(GPUComputePipeline const* pipeline) = 0;
    virtual void set_push_constant(GPURootSignature const* root_signature, String const& name, void const* data) = 0;
    virtual void dispatch(uint32_t x, uint32_t y, uint32_t z) = 0;
};

AMAZING_NAMESPACE_END


#endif //COMPUTE_PASS_ENCODER_H
