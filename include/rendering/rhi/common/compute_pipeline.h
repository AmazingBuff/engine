//
// Created by AmazingBuff on 2025/5/26.
//

#ifndef COMPUTE_PIPELINE_H
#define COMPUTE_PIPELINE_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUComputePipeline
{
public:
    GPUComputePipeline() : m_ref_root_signature(nullptr) {}
    virtual ~GPUComputePipeline() = default;

protected:
    GPURootSignature const* m_ref_root_signature;
};

AMAZING_NAMESPACE_END

#endif //COMPUTE_PIPELINE_H
