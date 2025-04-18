//
// Created by AmazingBuff on 2025/4/11.
//

#ifndef DX12DEVICE_H
#define DX12DEVICE_H

#include "rendering/rhi/common/device.h"
#include "d3d12ma/D3D12MemAlloc.h"

AMAZING_NAMESPACE_BEGIN

class DX12DescriptorHeap;

class DX12Device final : public GPUDevice
{
public:
    DX12Device();
    ~DX12Device() override;

    AResult initialize(GPUAdapter const* adapter, GPUDeviceCreateInfo const& info) override;

private:
    ID3D12Device*                   m_device;
    D3D12MA::Allocator*             m_allocator;
    Vector<ID3D12CommandQueue*>     m_command_queues[GPU_Queue_Type_Count];
    DX12DescriptorHeap*             m_descriptor_heap;
    ID3D12PipelineLibrary*          m_pipeline_library;

    friend class DX12SwapChain;
    friend class DX12Fence;
    friend class DX12Semaphore;
    friend class DX12Queue;
    friend class DX12CommandPool;
    friend class DX12CommandBuffer;
    friend class DX12QueryPool;
};

AMAZING_NAMESPACE_END

#endif //DX12DEVICE_H
