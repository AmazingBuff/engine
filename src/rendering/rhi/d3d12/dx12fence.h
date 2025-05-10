//
// Created by AmazingBuff on 2025/4/11.
//

#ifndef DX12FENCE_H
#define DX12FENCE_H

#include "dx12.h"
#include "rendering/rhi/common/fence.h"

AMAZING_NAMESPACE_BEGIN

class DX12Fence final : public GPUFence
{
public:
    explicit DX12Fence(GPUDevice const* device);
    ~DX12Fence() override;

    void wait() override;
private:
    ID3D12Fence*    m_fence;
    size_t          m_fence_value;
    HANDLE          m_wait_event;

    friend class DX12Queue;
};

AMAZING_NAMESPACE_END

#endif //DX12FENCE_H
