//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef DX12ADAPTER_H
#define DX12ADAPTER_H

#include "rendering/rhi/common/adapter.h"

AMAZING_NAMESPACE_BEGIN

class DX12Adapter final : public GPUAdapter
{
public:
    ~DX12Adapter() override;
private:
    DX12Adapter();
    void record_adapter_detail();

private:
    struct D3D12AdapterDetail
    {
        bool support_enhanced_barriers : 1;
    };

    IDXGIAdapter4*          m_adapter;
    D3D_FEATURE_LEVEL       m_feature_level;
    D3D12AdapterDetail      m_d3d12_detail;

    friend class DX12Instance;
    friend class DX12Device;
};

AMAZING_NAMESPACE_END

#endif //DX12ADAPTER_H
