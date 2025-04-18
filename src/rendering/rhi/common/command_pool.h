//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef COMMAND_POOL_H
#define COMMAND_POOL_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUCommandPool
{
public:
    GPUCommandPool() : m_type(GPUQueueType::e_graphics) {}
    virtual ~GPUCommandPool() = 0;

    virtual AResult initialize(GPUDevice const* device, GPUQueue const* queue) = 0;
protected:
    GPUQueueType m_type;
};

AMAZING_NAMESPACE_END

#endif //COMMAND_POO_H
