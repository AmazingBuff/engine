//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef DX12ADAPTER_H
#define DX12ADAPTER_H

#include "dx12.h"
#include "rendering/rhi/common/adapter.h"
#include <dxgi1_6.h>

AMAZING_NAMESPACE_BEGIN

class DX12Adapter final : public GPUAdapter
{
public:
    ~DX12Adapter() override;
    void query_memory_usage(uint64_t* total, uint64_t* used) override;

public:
    struct D3D12AdapterDetail
    {
        uint32_t support_enhanced_barriers : 1;
        D3D12_TILED_RESOURCES_TIER tiled_resource_tier;
    };
    D3D12AdapterDetail      m_d3d12_detail;
private:
    DX12Adapter();
    void record_adapter_detail();

private:
    IDXGIAdapter4*          m_adapter;
    D3D_FEATURE_LEVEL       m_feature_level;

    friend class DX12Instance;
    friend class DX12Device;
};

AMAZING_NAMESPACE_END

#endif //DX12ADAPTER_H
