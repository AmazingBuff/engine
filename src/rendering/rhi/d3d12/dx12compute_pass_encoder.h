//
// Created by AmazingBuff on 2025/5/26.
//

#ifndef DX12COMPUTE_PASS_ENCODER_H
#define DX12COMPUTE_PASS_ENCODER_H

#include "dx12.h"
#include "rendering/rhi/common/compute_pass_encoder.h"

AMAZING_NAMESPACE_BEGIN

class DX12CommandBuffer;

class DX12ComputePassEncoder final : GPUComputePassEncoder
{
public:
    void bind_descriptor_set(GPUDescriptorSet const* set) override;
    void bind_pipeline(GPUComputePipeline const* pipeline) override;
    void set_push_constant(GPURootSignature const* root_signature, String const& name, void const* data) override;
    void dispatch(uint32_t x, uint32_t y, uint32_t z) override;
private:
    DX12ComputePassEncoder();

private:
    DX12CommandBuffer* m_command_buffer;

    friend class DX12CommandBuffer;
};

AMAZING_NAMESPACE_END

#endif //DX12COMPUTE_PASS_ENCODER_H
