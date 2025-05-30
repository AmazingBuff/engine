//
// Created by AmazingBuff on 2025/5/26.
//

#ifndef DX12COMPUTE_PIPELINE_H
#define DX12COMPUTE_PIPELINE_H

#include "dx12.h"
#include "rendering/rhi/common/compute_pipeline.h"

AMAZING_NAMESPACE_BEGIN

class DX12ComputePipeline final : public GPUComputePipeline
{
public:
    explicit DX12ComputePipeline(GPUComputePipelineCreateInfo const& info);
    ~DX12ComputePipeline() override;

private:
    ID3D12PipelineState* m_pipeline_state;

    friend class DX12CommandBuffer;
    friend class DX12ComputePassEncoder;
};

AMAZING_NAMESPACE_END

#endif //DX12COMPUTE_PIPELINE_H
