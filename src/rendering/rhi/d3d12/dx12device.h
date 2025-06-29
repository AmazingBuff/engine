//
// Created by AmazingBuff on 2025/4/11.
//

#ifndef DX12DEVICE_H
#define DX12DEVICE_H

#include "rendering/rhi/common/device.h"
#include "d3d12ma/D3D12MemAlloc.h"

AMAZING_NAMESPACE_BEGIN

class DX12DescriptorHeap;
class DX12MemoryPool;
class DX12Queue;

class DX12Device final : public GPUDevice
{
public:
    DX12Device(GPUAdapter const* adapter, GPUDeviceCreateInfo const& info);
    ~DX12Device() override;

    [[nodiscard]] GPUQueue const* fetch_queue(GPUQueueType type, uint32_t index) const override;
private:
    ID3D12Device* m_device;
    D3D12MA::Allocator* m_allocator;
    Vector<DX12Queue*>  m_command_queues[GPU_Queue_Type_Count];
    DX12DescriptorHeap* m_descriptor_heap;
    ID3D12PipelineLibrary* m_pipeline_library;
    DX12MemoryPool* m_memory_pool;

    friend class DX12SwapChain;
    friend class DX12Fence;
    friend class DX12Semaphore;
    friend class DX12CommandPool;
    friend class DX12CommandBuffer;
    friend class DX12QueryPool;
    friend class DX12RootSignature;
    friend class DX12DescriptorSet;
    friend class DX12GraphicsPipeline;
    friend class DX12ComputePipeline;

    friend class DX12MemoryPool;
    friend class DX12Buffer;
    friend class DX12Texture;
    friend class DX12TextureView;
    friend class DX12Sampler;
};

AMAZING_NAMESPACE_END

#endif //DX12DEVICE_H
