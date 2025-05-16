//
// Created by AmazingBuff on 2025/4/25.
//

#ifndef DX12GRAPHICS_PIPELINE_H
#define DX12GRAPHICS_PIPELINE_H

#include "dx12.h"
#include "rendering/rhi/common/graphics_pipeline.h"

AMAZING_NAMESPACE_BEGIN

class DX12GraphicsPipeline final : public GPUGraphicsPipeline
{
public:
    explicit DX12GraphicsPipeline(GPUGraphicsPipelineCreateInfo const& info);
    ~DX12GraphicsPipeline() override;
private:
    ID3D12PipelineState* m_pipeline_state;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipeline_state_desc;
    D3D_PRIMITIVE_TOPOLOGY m_primitive_topology;

    friend class DX12GraphicsPassEncoder;
};

AMAZING_NAMESPACE_END

#endif //DX12GRAPHICS_PIPELINE_H
