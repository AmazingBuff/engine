//
// Created by AmazingBuff on 2025/4/18.
//

#ifndef QUERY_POOL_H
#define QUERY_POOL_H

#include "device.h"
#include "rendering/rhi/create_info.h"

AMAZING_NAMESPACE_BEGIN

class GPUQueryPool
{
public:
    GPUQueryPool() = default;
    virtual ~GPUQueryPool() = 0;

    virtual AResult initialize(GPUDevice const* device, GPUQueryPoolCreateInfo const& info) = 0;
};

AMAZING_NAMESPACE_END

#endif //QUERY_POOL_H
