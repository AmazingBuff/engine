//
// Created by AmazingBuff on 2025/4/23.
//

#ifndef SHADER_LIBRARY_H
#define SHADER_LIBRARY_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUShaderLibrary
{
public:
    GPUShaderLibrary() : m_ref_device(nullptr) {}
    virtual ~GPUShaderLibrary() = default;
protected:
    GPUDevice const* m_ref_device;
    Vector<GPUShaderReflection> m_shader_reflections;

    friend class GPURootSignature;
};

AMAZING_NAMESPACE_END


#endif //SHADER_LIBRARY_H
