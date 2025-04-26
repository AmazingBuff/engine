//
// Created by AmazingBuff on 2025/4/16.
//

#ifndef DX12QUEUE_H
#define DX12QUEUE_H

#include "rendering/rhi/common/queue.h"

AMAZING_NAMESPACE_BEGIN

class DX12Fence;

class DX12Queue final : public GPUQueue
{
public:
    ~DX12Queue() override;
private:
    DX12Queue();
private:
    ID3D12CommandQueue* m_queue;
    DX12Fence* m_fence;

    friend class DX12Device;
    friend class DX12SwapChain;
    friend class DX12CommandPool;
};

AMAZING_NAMESPACE_END

#endif //DX12QUEUE_H
