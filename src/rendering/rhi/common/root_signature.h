//
// Created by AmazingBuff on 2025/4/23.
//

#ifndef ROOT_SIGNATURE_H
#define ROOT_SIGNATURE_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPURootSignature
{
public:
    GPURootSignature() : m_pipeline_type(GPUPipelineType::e_graphics), m_pool(nullptr) {}
    virtual ~GPURootSignature();

    virtual AResult initialize(GPUDevice const* device, GPURootSignatureCreateInfo const& info);
protected:
    Vector<GPUParameterTable> m_tables;
    Vector<GPUShaderResource> m_push_constants;
    Vector<GPUShaderResource> m_static_samplers;
    GPUPipelineType m_pipeline_type;
    GPURootSignaturePool* m_pool;
private:
    friend class GPURootSignaturePool;
};

AMAZING_NAMESPACE_END

#endif //ROOT_SIGNATURE_H
