//
// Created by AmazingBuff on 2025/4/23.
//

#ifndef DX12SHADER_LIBRARY_H
#define DX12SHADER_LIBRARY_H

#include "rendering/rhi/common/shader_library.h"
#include <dxc/dxcapi.h>

AMAZING_NAMESPACE_BEGIN

class DX12ShaderLibrary final : public GPUShaderLibrary
{
public:
    DX12ShaderLibrary(GPUDevice const* device, GPUShaderLibraryCreateInfo const& info);
    ~DX12ShaderLibrary() override;
private:
    IDxcBlobEncoding* m_blob_encoding;

    friend class DX12GraphicsPipeline;
    friend class DX12ComputePipeline;
};


AMAZING_NAMESPACE_END
#endif //DX12SHADER_LIBRARY_H
