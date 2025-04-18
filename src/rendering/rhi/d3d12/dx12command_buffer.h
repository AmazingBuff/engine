//
// Created by AmazingBuff on 2025/4/17.
//

#ifndef DX12COMMAND_BUFFER_H
#define DX12COMMAND_BUFFER_H

#include "rendering/rhi/common/command_buffer.h"
#include "internal/dx12descriptor_heap.h"

AMAZING_NAMESPACE_BEGIN

class DX12CommandBuffer final : public GPUCommandBuffer
{
public:
    DX12CommandBuffer();
    ~DX12CommandBuffer() override;

    AResult initialize(GPUDevice const* device, GPUCommandPool const* pool, GPUCommandBufferCreateInfo const& info) override;

    void begin_command() override;
    void end_command() override;

    void begin_query(GPUQueryPool const* pool, GPUQueryInfo const& info) override;
    void end_query(GPUQueryPool const* pool, GPUQueryInfo const& info) override;
private:
    ID3D12GraphicsCommandList* m_command_list;
    // cbv srv uav heap, sampler heap
    const DX12DescriptorHeap::D3D12DescriptorHeap* m_bound_descriptor_heaps[2];
    // for multi device rendering
    uint32_t m_bound_heap_index;

    const ID3D12RootSignature* m_bound_root_signature;
};

AMAZING_NAMESPACE_END

#endif //DX12COMMAND_BUFFER_H
