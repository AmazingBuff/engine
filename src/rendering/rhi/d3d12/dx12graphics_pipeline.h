//
// Created by AmazingBuff on 2025/4/25.
//

#ifndef DX12GRAPHICS_PIPELINE_H
#define DX12GRAPHICS_PIPELINE_H

#include "rendering/rhi/common/graphics_pipeline.h"

AMAZING_NAMESPACE_BEGIN

class DX12GraphicsPipeline final : public GPUGraphicsPipeline
{
public:
    DX12GraphicsPipeline();
    ~DX12GraphicsPipeline() override;

    AResult initialize(GPUDevice const* device, GPUGraphicsPipelineCreateInfo const& info) override;
private:
    ID3D12RootSignature* m_root_signature;
    ID3D12PipelineState* m_pipeline_state;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipeline_state_desc;
    D3D_PRIMITIVE_TOPOLOGY m_primitive_topology;
};

AMAZING_NAMESPACE_END

#endif //DX12GRAPHICS_PIPELINE_H
