//
// Created by AmazingBuff on 2025/4/24.
//

#ifndef DX12ROOT_SIGNATURE_H
#define DX12ROOT_SIGNATURE_H

#include "dx12.h"
#include "rendering/rhi/common/root_signature.h"

AMAZING_NAMESPACE_BEGIN

class DX12RootSignature final : public GPURootSignature
{
public:
    DX12RootSignature(GPUDevice const* device, GPURootSignatureCreateInfo const& info);
    ~DX12RootSignature() override;
private:
    ID3D12RootSignature* m_root_signature;

    struct D3D12ConstantParameter
    {
        D3D12_ROOT_PARAMETER1 root_parameter;
        uint32_t index;
    };

    Vector<D3D12ConstantParameter> m_constant_parameters;

    friend class DX12DescriptorSet;
    friend class DX12GraphicsPipeline;
    friend class DX12GraphicsPassEncoder;
};

AMAZING_NAMESPACE_END

#endif //DX12ROOT_SIGNATURE_H
