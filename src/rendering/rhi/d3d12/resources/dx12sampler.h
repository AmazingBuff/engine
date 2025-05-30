//
// Created by AmazingBuff on 2025/4/22.
//

#ifndef DX12SAMPLER_H
#define DX12SAMPLER_H

#include "rendering/rhi/d3d12/dx12.h"
#include "rendering/rhi/common/sampler.h"

AMAZING_NAMESPACE_BEGIN

class DX12Sampler final : public GPUSampler
{
public:
    DX12Sampler(GPUDevice const* device, GPUSamplerCreateInfo const& info);
    ~DX12Sampler() override;
private:
    D3D12_SAMPLER_DESC m_desc;
    D3D12_CPU_DESCRIPTOR_HANDLE m_handle;

    friend class DX12RootSignature;
    friend class DX12DescriptorSet;
};

AMAZING_NAMESPACE_END

#endif //DX12SAMPLER_H
