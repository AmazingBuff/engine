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
    GPUQueue() : m_type(GPUQueueType::e_graphics) {}
    virtual ~GPUQueue() = 0;

    virtual AResult initialize(GPUDevice const* device, GPUQueueType type, uint32_t index) = 0;
protected:
    GPUQueueType m_type;
};

AMAZING_NAMESPACE_END

#endif //QUEUE_H
