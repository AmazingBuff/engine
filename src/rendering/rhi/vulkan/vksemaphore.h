//
// Created by AmazingBuff on 25-5-10.
//

#pragma once

#include "vk.h"
#include "rendering/rhi/common/semaphore.h"

AMAZING_NAMESPACE_BEGIN

class VKSemaphore final : public GPUSemaphore
{
public:
    explicit VKSemaphore(GPUDevice const* device);
    ~VKSemaphore() override;
private:
    VkSemaphore m_semaphore;

    friend class VKSwapChain;
    friend class VKQueue;
};


AMAZING_NAMESPACE_END