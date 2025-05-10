//
// Created by AmazingBuff on 25-5-10.
//

#pragma once

#include "vk.h"
#include "rendering/rhi/common/fence.h"

AMAZING_NAMESPACE_BEGIN

class VKFence final : public GPUFence
{
public:
    explicit VKFence(GPUDevice const* device);
    ~VKFence() override;

    void wait() override;
private:
    VkFence m_fence;

    friend class VKSwapChain;
    friend class VKQueue;
};

AMAZING_NAMESPACE_END