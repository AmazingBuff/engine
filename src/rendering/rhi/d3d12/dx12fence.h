//
// Created by AmazingBuff on 2025/4/11.
//

#ifndef DX12FENCE_H
#define DX12FENCE_H

#include "rendering/rhi/common/fence.h"

AMAZING_NAMESPACE_BEGIN

class DX12Fence final : public GPUFence
{
public:
    DX12Fence();
    ~DX12Fence() override;

    AResult initialize(GPUDevice const* device) override;
    AResult wait() override;
private:
    ID3D12Fence*    m_fence;
    size_t          m_fence_value;
    HANDLE          m_wait_event;
};

AMAZING_NAMESPACE_END

#endif //DX12FENCE_H
