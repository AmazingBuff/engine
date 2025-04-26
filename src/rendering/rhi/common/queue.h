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
    virtual ~GPUQueue() = default;
protected:
    GPUQueueType m_type;
};

AMAZING_NAMESPACE_END

#endif //QUEUE_H
