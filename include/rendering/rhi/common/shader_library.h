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
    GPUShaderLibrary() = default;
    virtual ~GPUShaderLibrary() = default;
protected:
    Vector<GPUShaderReflection> m_shader_reflections;

    friend class GPURootSignature;
};

AMAZING_NAMESPACE_END


#endif //SHADER_LIBRARY_H
