//
// Created by AmazingBuff on 2025/4/14.
//

#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUCommandBuffer
{
public:
    GPUCommandBuffer() : m_ref_pool(nullptr) {}
    virtual ~GPUCommandBuffer() = default;

    virtual AResult initialize(GPUDevice const* device, GPUCommandPool const* pool, GPUCommandBufferCreateInfo const& info) = 0;
    virtual void begin_command() = 0;
    virtual void end_command() = 0;

    virtual void begin_query(GPUQueryPool const* pool, GPUQueryInfo const& info) = 0;
    virtual void end_query(GPUQueryPool const* pool, GPUQueryInfo const& info) = 0;
protected:
    GPUCommandPool const* m_ref_pool;
};

AMAZING_NAMESPACE_END

#endif //COMMAND_BUFFER_H
