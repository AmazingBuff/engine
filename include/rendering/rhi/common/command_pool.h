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
    GPUCommandPool() : m_ref_device(nullptr), m_type(GPUQueueType::e_graphics) {}
    virtual ~GPUCommandPool() = default;

    virtual void reset() = 0;
protected:
    GPUDevice const* m_ref_device;
    GPUQueueType m_type;
};

AMAZING_NAMESPACE_END

#endif //COMMAND_POO_H
