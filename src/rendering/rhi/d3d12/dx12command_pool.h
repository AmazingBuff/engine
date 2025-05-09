//
// Created by AmazingBuff on 2025/4/17.
//

#ifndef DX12COMMAND_POOL_H
#define DX12COMMAND_POOL_H

#include "dx12.h"
#include "rendering/rhi/common/command_pool.h"

AMAZING_NAMESPACE_BEGIN

class DX12CommandPool final : public GPUCommandPool
{
public:
    DX12CommandPool(GPUDevice const* device, GPUQueue const* queue);
    ~DX12CommandPool() override;

    void reset() override;
private:
    ID3D12CommandAllocator* m_command_allocator;

    friend class DX12CommandBuffer;
};

AMAZING_NAMESPACE_END

#endif //DX12COMMAND_POOL_H
