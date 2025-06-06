//
// Created by AmazingBuff on 2025/4/16.
//

#ifndef QUEUE_H
#define QUEUE_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUQueue
{
public:
    GPUQueue() : m_ref_device(nullptr), m_type(GPUQueueType::e_graphics) {}
    virtual ~GPUQueue() = default;

    virtual void submit(GPUQueueSubmitInfo const& info) const = 0;
    virtual void wait_idle() const = 0;
    virtual void present(GPUQueuePresentInfo const& info) const = 0;
protected:
    GPUDevice const* m_ref_device;
    GPUQueueType m_type;
};

AMAZING_NAMESPACE_END

#endif //QUEUE_H
