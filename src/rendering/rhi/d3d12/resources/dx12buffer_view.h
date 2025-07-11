//
// Created by AmazingBuff on 2025/7/7.
//

#ifndef DX12BUFFER_VIEW_H
#define DX12BUFFER_VIEW_H

#include "rendering/rhi/d3d12/dx12.h"
#include "rendering/rhi/common/buffer_view.h"

AMAZING_NAMESPACE_BEGIN

class DX12BufferView final : public GPUBufferView
{
public:
    explicit DX12BufferView(GPUBufferViewCreateInfo const& info);
    ~DX12BufferView() override;
private:
    // buffer address in cpu, max num for three buffers of different type
    D3D12_CPU_DESCRIPTOR_HANDLE m_handle;
    // order: cbv -- srv -- uav
    uint32_t m_srv_offset;
    uint32_t m_uav_offset;

    friend class DX12DescriptorSet;
};

AMAZING_NAMESPACE_END

#endif //DX12BUFFER_VIEW_H
