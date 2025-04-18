//
// Created by AmazingBuff on 2025/4/16.
//

#ifndef DX12SEMAPHORE_H
#define DX12SEMAPHORE_H

#include "rendering/rhi/common/semaphore.h"

AMAZING_NAMESPACE_BEGIN


class DX12Semaphore final : public GPUSemaphore
{
public:
    DX12Semaphore();
    ~DX12Semaphore() override;

    AResult initialize(GPUDevice const* device) override;
private:
    ID3D12Fence*    m_fence;
    size_t          m_fence_value;
    HANDLE          m_wait_event;
};

AMAZING_NAMESPACE_END

#endif //DX12SEMAPHORE_H
