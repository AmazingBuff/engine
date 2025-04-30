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

    void map(size_t offset, size_t size) override;
    void unmap() override;
private:
    ID3D12Resource*  m_resource;
    D3D12MA::Allocation* m_allocation;
    // gpu address
    D3D12_GPU_VIRTUAL_ADDRESS m_gpu_address;
    // buffer address in cpu, max num for three buffers of different type
    D3D12_CPU_DESCRIPTOR_HANDLE m_handle;
    // order: cbv -- srv -- uav
    uint32_t m_srv_offset;
    uint32_t m_uav_offset;

    friend class DX12DescriptorSet;
    friend class DX12CommandBuffer;
    friend class DX12GraphicsPassEncoder;
};

AMAZING_NAMESPACE_END

#endif //DX12BUFFER_H
