//
// Created by AmazingBuff on 2025/4/25.
//

#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUGraphicsPipeline
{
public:
    GPUGraphicsPipeline();
    virtual ~GPUGraphicsPipeline() = 0;

    virtual AResult initialize(GPUDevice const* device, GPUGraphicsPipelineCreateInfo const& info) = 0;
protected:
    GPUDevice const* m_ref_device;
    GPURootSignature const* m_ref_root_signature;
};

AMAZING_NAMESPACE_END

#endif //GRAPHICS_PIPELINE_H
