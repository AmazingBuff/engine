//
// Created by AmazingBuff on 2025/4/22.
//

#ifndef DX12TEXTURE_VIEW_H
#define DX12TEXTURE_VIEW_H

#include "rendering/rhi/d3d12/dx12.h"
#include "rendering/rhi/common/texture_view.h"

AMAZING_NAMESPACE_BEGIN

class DX12TextureView final : public GPUTextureView
{
public:
    DX12TextureView(GPUDevice const* device, GPUTextureViewCreateInfo const& info);
    ~DX12TextureView() override;
private:
    D3D12_CPU_DESCRIPTOR_HANDLE m_rtv_dsv_handle;
    D3D12_CPU_DESCRIPTOR_HANDLE m_srv_uva_handle;
    // order: srv -- uav
    uint32_t m_uav_offset;

    friend class DX12DescriptorSet;
    friend class DX12CommandBuffer;
};

AMAZING_NAMESPACE_END

#endif //DX12TEXTURE_VIEW_H
