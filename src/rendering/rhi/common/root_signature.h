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
    GPURootSignature();
    virtual ~GPURootSignature();
    [[nodiscard]] GPUResourceState fetch_shader_resource_state(const String& name) const;
protected:
    void initialize(GPURootSignatureCreateInfo const& info);
protected:
    GPUDevice const* m_ref_device;
    GPURootSignaturePool* m_pool;

    Vector<GPUParameterTable> m_tables;
    Vector<GPUShaderResource> m_static_samplers;
    // for derived
    GPUShaderResource* m_push_constants;
    uint32_t m_push_constant_count;
    GPUPipelineType m_pipeline_type;
private:
    friend class GPURootSignaturePool;
};

AMAZING_NAMESPACE_END

#endif //ROOT_SIGNATURE_H
