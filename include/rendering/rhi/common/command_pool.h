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
    GPUCommandPool() : m_ref_queue(nullptr) {}
    virtual ~GPUCommandPool() = default;

    virtual void reset() = 0;
protected:
    GPUQueue const* m_ref_queue;
};

AMAZING_NAMESPACE_END

#endif //COMMAND_POOL_H
