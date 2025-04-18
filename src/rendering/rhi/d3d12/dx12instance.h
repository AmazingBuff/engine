//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef DX12INSTANCE_H
#define DX12INSTANCE_H

#include "rendering/rhi/common/instance.h"

AMAZING_NAMESPACE_BEGIN

class DX12Adapter;

class DX12Instance final : public GPUInstance
{
public:
    DX12Instance();
    ~DX12Instance() override;

    AResult initialize(GPUInstanceCreateInfo const& info) override;

    void enum_adapters(const GPUAdapter** const adapters, uint32_t* num_adapters) const override;
private:
    IDXGIFactory6*          m_dxgi_factory;
    ID3D12Debug*            m_debug;
    Vector<DX12Adapter>     m_adapters;

    friend class DX12SwapChain;
};

AMAZING_NAMESPACE_END

#endif //DX12INSTANCE_H
