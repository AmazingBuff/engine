//
// Created by AmazingBuff on 2025/4/18.
//

#ifndef DX12BUFFER_H
#define DX12BUFFER_H

#include "rendering/rhi/common/buffer.h"
#include "rendering/rhi/d3d12/d3d12ma/D3D12MemAlloc.h"

AMAZING_NAMESPACE_BEGIN

class DX12Buffer final : public GPUBuffer
{
public:
    DX12Buffer(GPUDevice const* device, GPUBufferCreateInfo const& info);
    ~DX12Buffer() override;

    void map(size_t offset, size_t size, const void* data) const override;
    void unmap() const override;
private:
    ID3D12Resource*  m_resource;
    D3D12MA::Allocation* m_allocation;

    friend class DX12BufferView;
    friend class DX12CommandBuffer;
    friend class DX12GraphicsPassEncoder;
};

AMAZING_NAMESPACE_END

#endif //DX12BUFFER_H
