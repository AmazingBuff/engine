//
// Created by AmazingBuff on 25-5-10.
//

#pragma once

#include "vk.h"
#include "rendering/rhi/common/queue.h"

AMAZING_NAMESPACE_BEGIN

class VKFence;

class VKQueue final : public GPUQueue
{
public:
    ~VKQueue() override;

    void submit(GPUQueueSubmitInfo const& info) const override;
    void wait_idle() const override;
    void present(GPUQueuePresentInfo const& info) const override;
private:
    VKQueue();
private:
    VkQueue m_queue;

    friend class VKDevice;
    friend class VKCommandPool;
};

AMAZING_NAMESPACE_END